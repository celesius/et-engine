/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/rendering/rendercontext.h>
#include <et/gui/layout.h>

using namespace et;
using namespace et::gui;

Layout::Layout() : Element2d(0),
	_currentElement(0), _focusedElement(0), _capturedElement(0), _valid(false), _dragging(false)
{
}

void Layout::layout(const vec2& sz)
{
	setFrame(vec2(0.0f), sz);
	layoutChildren();
}

void Layout::adjustVerticalOffset(float) { }
void Layout::resetVerticalOffset() { }

void Layout::addElementToRenderQueue(Element* element, RenderContext* rc, GuiRenderer& gr)
{
	if (!element->visible()) return;

	bool clipToBounds = element->hasFlag(ElementFlag_ClipToBounds);

	if (clipToBounds)
	{
		mat4 parentTransform = element->parent()->finalTransform();
		vec2 eSize = multiplyWithoutTranslation(element->size(), parentTransform);
		vec2 eOrigin = parentTransform * element->origin();
		
		gr.pushClipRect(recti(vec2i(static_cast<int>(eOrigin.x),
			static_cast<int>(rc->size().y - eOrigin.y - eSize.y)),
			vec2i(static_cast<int>(eSize.x), static_cast<int>(eSize.y))));
	}
	
	element->addToRenderQueue(rc, gr);

	ET_ITERATE(element->children(), auto&, i, addElementToRenderQueue(i.ptr(), rc, gr))

	element->addToOverlayRenderQueue(rc, gr);

	if (clipToBounds)
	{
		gr.popClipRect();
	}
}

void Layout::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	gr.resetClipRect();

	ET_ITERATE(children(), auto&, i, addElementToRenderQueue(i.ptr(), rc, gr))
	ET_ITERATE(_topmostElements, auto&, i, i->addToRenderQueue(rc, gr))

	_valid = true;
}

bool Layout::pointerPressed(const et::PointerInputInfo& p)
{ 
	if (hasFlag(ElementFlag_TransparentForPointer)) return false;

	if (_capturedElement)
	{
		_capturedElement->pointerPressed(PointerInputInfo(p.type, _capturedElement->positionInElement(p.pos), 
			p.normalizedPos, p.scroll, p.id, p.timestamp));
		return true;
	}
	else 
	{
		Element* active = activeElement(p);
		setCurrentElement(p, active);
		bool processed = false;
		if (!active)
		{
			setActiveElement(0);
		}
		else
		{
			processed = active->pointerPressed(PointerInputInfo(p.type, active->positionInElement(p.pos),
				p.normalizedPos, p.scroll, p.id, p.timestamp));

			if ((p.type == PointerType_General))
			{
				if (active->hasFlag(ElementFlag_Dragable))
				{
					processed = true;
					_capturedElement = active;
					_dragging = true;
					_dragInitialPosition = active->position();
					_dragInitialOffset = active->positionInElement(p.pos);
					
					_capturedElement->dragStarted.invoke(_capturedElement,
						ElementDragInfo(_dragInitialPosition, _dragInitialPosition, p.normalizedPos));

					if (input().canGetCurrentPointerInfo())
						startUpdates();
				}
				else
				{
					setActiveElement(active);
				}
			}

			if (processed || _dragging)
				_capturedElement = active;
		}

		return processed;
	}
}

bool Layout::pointerMoved(const et::PointerInputInfo& p)
{ 
	if (hasFlag(ElementFlag_TransparentForPointer)) return false;

	if (_capturedElement)
	{
		if (!input().canGetCurrentPointerInfo() && (p.type == PointerType_General) && _dragging)
		{
			vec2 currentPos = _capturedElement->parent()->positionInElement(p.pos);
			
			_capturedElement->setPosition(currentPos - _dragInitialOffset);
			_capturedElement->dragged.invoke(_capturedElement,
				ElementDragInfo(currentPos, _dragInitialPosition, p.normalizedPos));
		}

		_capturedElement->pointerMoved(PointerInputInfo(p.type,
			_capturedElement->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp));

		return true;
	}
	else 
	{
		bool processed = false;

		Element* active = activeElement(p);
		setCurrentElement(p, active);

		if (active)
		{
			processed = active->pointerMoved(PointerInputInfo(p.type, active->positionInElement(p.pos), 
				p.normalizedPos, p.scroll, p.id, p.timestamp));
		}

		return processed;
	}
}

bool Layout::pointerReleased(const et::PointerInputInfo& p)
{ 
	if (hasFlag(ElementFlag_TransparentForPointer)) return false;

	Element* active = activeElement(p);
	if (_capturedElement)
	{
		bool processed = _capturedElement->pointerReleased(PointerInputInfo(p.type, _capturedElement->positionInElement(p.pos), 
			p.normalizedPos, p.scroll, p.id, p.timestamp));

		if ((p.type == PointerType_General) && _dragging)
		{
			if (input().canGetCurrentPointerInfo())
				cancelUpdates();

			vec2 currentPos = _capturedElement->parent()->positionInElement(p.pos);
			_capturedElement->dragFinished.invoke(_capturedElement, ElementDragInfo(currentPos, _dragInitialPosition, p.normalizedPos));
			_dragging = false;
			_capturedElement = 0;
		}
		else if (processed) 
		{
			_capturedElement = 0;
		}

		return true;
	}
	else 
	{
		setCurrentElement(p, active);
		bool processed = false;

		if (active)
		{
			processed = active->pointerReleased(PointerInputInfo(p.type, active->positionInElement(p.pos), 
				p.normalizedPos, p.scroll, p.id, p.timestamp));
		}

		return processed;
	}
}

bool Layout::pointerScrolled(const et::PointerInputInfo& p)
{ 
	if (hasFlag(ElementFlag_TransparentForPointer)) return false;

	if (_capturedElement)
	{
		_capturedElement->pointerScrolled(PointerInputInfo(p.type, _capturedElement->positionInElement(p.pos),
			p.normalizedPos, p.scroll, p.id, p.timestamp));
		return true;
	}
	else 
	{
		Element* active = activeElement(p);
		setCurrentElement(p, active);
		bool processed = false;
		if (active)
		{
			processed = active->pointerScrolled(PointerInputInfo(p.type, active->positionInElement(p.pos), 
				p.normalizedPos, p.scroll, p.id, p.timestamp));
		}
		return processed;
	}
}

Element* Layout::activeElement(const PointerInputInfo& p)
{
	if (!_valid)
	{
		_topmostElements.clear();
		ET_ITERATE(children(), auto&, i, collectTopmostElements(i.ptr()))
	}

	Element* active = 0;
	for (auto i = _topmostElements.rbegin(), e = _topmostElements.rend(); i != e; ++i)
	{
		active = getActiveElement(p, i->ptr());
		if (active)	break;
	}

	if (!active)
	{
		for (auto i = children().rbegin(), e = children().rend(); i != e; ++i)
		{
			active = getActiveElement(p, i->ptr());
			if (active)	break;
		}
	}

	return active;
}

Element* Layout::getActiveElement(const PointerInputInfo& p, Element* el)
{
	if (!el->visible() || !el->enabled() || !el->containsPoint(p.pos, p.normalizedPos))
		return nullptr;
	
	if (el->hasFlag(ElementFlag_HandlesChildEvents))
		return el;

	for (auto ei = el->children().rbegin(), ee = el->children().rend(); ei != ee; ++ei)
	{
		Element* element = getActiveElement(p, ei->ptr());
		if (element)
			return element;
	}

	return el->hasFlag(ElementFlag_TransparentForPointer) ? nullptr : el;
}

void Layout::setCurrentElement(const PointerInputInfo& p, Element* e)
{
	if (e == _currentElement) return;

	if (_currentElement)
		_currentElement->pointerLeaved(p);

	_currentElement = e;

	if (_currentElement)
		_currentElement->pointerEntered(p);
}

void Layout::update(float)
{
	if (_dragging && input().canGetCurrentPointerInfo())
	{
		PointerInputInfo pi = input().currentPointer();
		vec2 currentPos = _capturedElement->parent()->positionInElement(pi.pos);
		if ((currentPos - _capturedElement->position()).dotSelf() > 0.01f)
		{
			_capturedElement->setPosition(0.5f * (currentPos + _capturedElement->position()));
			_capturedElement->dragged.invoke(_capturedElement,
				ElementDragInfo(currentPos, _dragInitialPosition, pi.normalizedPos));
		}
	}
}

void Layout::cancelDragging(float returnDuration)
{
	if (_dragging)
	{
		_capturedElement->setPosition(_dragInitialPosition, returnDuration);
		_capturedElement = 0;
		_dragging = false;
	}
}

void Layout::setActiveElement(Element* e)
{
	if (_focusedElement == e) return;
	
	if (_focusedElement)
		_focusedElement->resignFocus(e);

	_focusedElement = e;

	bool needKeyboard = _focusedElement && _focusedElement->hasFlag(ElementFlag_RequiresKeyboard);

	if (_focusedElement)
		_focusedElement->setFocus();
	
	if (needKeyboard)
		layoutRequiresKeyboard.invoke(this, _focusedElement);
	else
		layoutDoesntNeedKeyboard.invoke(this);
}

void Layout::setInvalid()
{
	_valid = false;
}

void Layout::collectTopmostElements(Element* element)
{
	if (!element->visible()) return;
	
	if (element->hasFlag(ElementFlag_RenderTopmost))
		_topmostElements.push_back(Element::Pointer(element));

	ET_ITERATE(element->children(), auto&, i, collectTopmostElements(i.ptr()))
}

void Layout::initRenderingElement(et::RenderContext* rc)
{
	if (_renderingElement.invalid())
		_renderingElement = RenderingElement::Pointer(new RenderingElement(rc));
}

/*
 *
 * Modal Layout
 *
 */

ModalLayout::ModalLayout()
{
	_backgroundFade = ImageView::Pointer(new ImageView(Texture(), this));
	_backgroundFade->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.25f));
}

bool ModalLayout::pointerPressed(const et::PointerInputInfo& p)
{
	Layout::pointerPressed(p);
	return true;
}

bool ModalLayout::pointerMoved(const et::PointerInputInfo& p)
{
	Layout::pointerMoved(p);
	return true;
}

bool ModalLayout::pointerReleased(const et::PointerInputInfo& p)
{
	Layout::pointerReleased(p);
	return true;
}

bool ModalLayout::pointerScrolled(const et::PointerInputInfo& p )
{
	Layout::pointerScrolled(p);
	return true;
}
