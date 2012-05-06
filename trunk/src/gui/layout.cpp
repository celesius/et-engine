/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/layout.h>

using namespace et;
using namespace et::gui;

Layout::Layout() : 
	Element2D(0), _currentElement(0), _focusedElement(0), _capturedElement(0), _valid(false), _dragging(false)
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
	
	if (!element->hasFlag(ElementFlag_RenderTopmost))
		element->addToRenderQueue(rc, gr);

	for (Element::List::iterator i = element->children().begin(), e = element->children().end(); i != e; ++i)
		addElementToRenderQueue(i->ptr(), rc, gr);
}

void Layout::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	PointerInputInfo p = input().currentPointer();
	Element* e = activeElement(p);

	if (_capturedElement)
	{
		if (e == _capturedElement)
			setCurrentElement(p, e);
	}
	else 
		setCurrentElement(p, e);

	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		addElementToRenderQueue(i->ptr(), rc, gr);

	for (Element::List::iterator i = _topmostElements.begin(), e = _topmostElements.end(); i != e; ++i)
		(*i)->addToRenderQueue(rc, gr);

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

			if ((active->capturesPointer() && processed) || _dragging)
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
			_capturedElement->setPosition(currentPos);
			_capturedElement->dragged.invoke(_capturedElement, ElementDragInfo(currentPos, _dragInitialPosition, p.normalizedPos));
		}

		_capturedElement->pointerMoved(PointerInputInfo(p.type, _capturedElement->positionInElement(p.pos), 
			p.normalizedPos, p.scroll, p.id, p.timestamp));

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
	Element* active = 0;

	for (Element::List::reverse_iterator i = _topmostElements.rbegin(), e = _topmostElements.rend(); i != e; ++i)
	{
		active = getActiveElement(p, i->ptr());
		if (active)	break;
	}

	if (!active)
	{
		for (Element::List::reverse_iterator i = children().rbegin(), e = children().rend(); i != e; ++i)
		{
			active = getActiveElement(p, i->ptr());
			if (active)	break;
		}
	}

	return active;
}

Element* Layout::getActiveElement(const PointerInputInfo& p, Element* e)
{
	if (!e->visible() || !e->enabled() || !e->containPoint(p.pos, p.normalizedPos)) return 0;

	for (Element::List::reverse_iterator ei = e->children().rbegin(), ee = e->children().rend(); ei != ee; ++ei)
	{
		Element* element = getActiveElement(p, ei->ptr());
		if (element)
			return element;
	}

	return e->hasFlag(ElementFlag_TransparentForPointer) ? 0 : e;
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
			_capturedElement->dragged.invoke(_capturedElement, ElementDragInfo(currentPos, _dragInitialPosition, pi.normalizedPos));
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
	_topmostElements.clear();

	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		collectTopmostElements(i->ptr());
}

void Layout::collectTopmostElements(Element* element)
{
	if (!element->visible()) return;
	
	if (element->hasFlag(ElementFlag_RenderTopmost))
		_topmostElements.push_back(Element::Pointer(element));

	for (Element::List::iterator i = element->children().begin(), e = element->children().end(); i != e; ++i)
		collectTopmostElements(i->ptr());
}