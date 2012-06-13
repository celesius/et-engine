/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/gui/gui.h>

using namespace et;
using namespace et::gui;


Gui::Gui(RenderContext* rc, TextureCache& texCache) : _rc(rc), _textureCache(texCache), 
	_renderer(rc, true), 
	_renderingElementBackground(new RenderingElement(rc)), 
	_renderingElementKeyboard(new RenderingElement(rc)), 
	_keyboard(rc, Font(rc, "ui/fonts/main.font", _textureCache), texCache), 
	_background(Texture(), 0), _backgroundValid(true)
{
	_background.setPivotPoint(vec2(0.5f));
	_background.setContentMode(ImageView::ContentMode_Stretch);
	
	layout(rc->size());
}

bool Gui::pointerPressed(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

	bool processed = _keyboard.visible() && _keyboard.pointerPressed(p);

	if (!processed)
	{
		for (LayoutEntryStack::reverse_iterator i = _layouts.rbegin(), e = _layouts.rend(); i != e; ++i)
		{
			if ((*i)->layout->pointerPressed(p))
				return true;
		}
	}

	return processed;
}

bool Gui::pointerMoved(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

	bool processed = _keyboard.visible() && _keyboard.pointerMoved(p);

	if (!processed)
	{
		for (LayoutEntryStack::reverse_iterator i = _layouts.rbegin(), e = _layouts.rend(); i != e; ++i)
		{
			if ((*i)->layout->pointerMoved(p))
				return true;
		}
	}

	return processed;
}

bool Gui::pointerReleased(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

	bool processed = _keyboard.visible() && _keyboard.pointerReleased(p);
	if (!processed)
	{
		for (LayoutEntryStack::reverse_iterator i = _layouts.rbegin(), e = _layouts.rend(); i != e; ++i)
		{
			if ((*i)->layout->pointerReleased(p))
				return true;
		}
	}

	return processed;
}

bool Gui::pointerScrolled(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

	bool processed = _keyboard.visible() && _keyboard.pointerScrolled(p);
	if (!processed)
	{
		for (LayoutEntryStack::reverse_iterator i = _layouts.rbegin(), e = _layouts.rend(); i != e; ++i)
		{
			if ((*i)->layout->pointerScrolled(p))
				return true;
		}
	}

	return processed;
}

void Gui::buildLayoutVertices(RenderContext* rc, RenderingElementRef& element, Layout::Pointer layout)
{
	_renderer.setRendernigElement(element);

	if (!layout->valid())
	{
		element->clear();
		layout->addToRenderQueue(rc, _renderer);
	}
}

void Gui::buildKeyboardVertices(RenderContext* rc)
{
	if (_keyboard.invalid())
	{
		_renderingElementKeyboard->clear();
		_keyboard.addToRenderQueue(rc, _renderer);
	}
}

void Gui::buildBackgroundVertices(RenderContext* rc)
{
	if (!_backgroundValid)
	{
		_renderingElementBackground->clear();
		_background.addToRenderQueue(rc, _renderer);
	}
	_backgroundValid = true;
}

void Gui::layout(const vec2& size)
{
	_screenSize = size;
	_renderer.setProjectionMatrices(size);
	_background.setFrame(0.5f * size, size);
	_backgroundValid = false;
	_keyboard.layout(size);

	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
		(*i)->layout->layout(size);
}

void Gui::render(RenderContext* rc)
{
	_renderer.beginRender(rc);

	if (_background.texture().valid())
	{
		_renderer.setRendernigElement(_renderingElementBackground);
		buildBackgroundVertices(rc);
		_renderer.setCustomAlpha(1.0f);
		_renderer.setCustomOffset(vec2(0.0f));
		_renderer.render(rc);
	}

	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		LayoutEntry& obj = *i;
		buildLayoutVertices(rc, obj->renderingElement, obj->layout);
		_renderer.setCustomAlpha(obj->offsetAlpha.z);
		_renderer.setCustomOffset(obj->offsetAlpha.xy());
		_renderer.render(rc);
	}

	if (_keyboard.visible())
	{
		_renderer.setRendernigElement(_renderingElementKeyboard);
		buildKeyboardVertices(rc);
		_renderer.setCustomAlpha(1.0f);
		_renderer.setCustomOffset(vec2(0.0f));
		_renderer.render(rc);
	}

	_renderer.endRender(rc);
}

void Gui::setBackgroundImage(const Image& img)
{
	_background.setImage(img);
	_backgroundValid = false;
}

void Gui::onKeyboardNeeded(Layout* l, Element* element)
{
	if (!platformHasHardwareKeyboard())
	{
		vec2 size = element->size();
		vec2 middlePoint = element->finalTransform() * vec2(0.0f);
		float dy = (middlePoint.y + 0.5f * size.y) - _keyboard.topOrigin();
		if (dy > 0.0f)
			l->adjustVerticalOffset(-dy - size.y);
	}

	_keyboard.show(true, element);
	_keyboard.setDelegate(l);
}

void Gui::onKeyboardResigned(Layout* l)
{
	if (!platformHasHardwareKeyboard() && _keyboard.visible())
		l->resetVerticalOffset();

	_keyboard.hide(true);
}


void Gui::getAnimationParams(size_t flags, vec3* nextSrc, vec3* nextDst, vec3* currDst)
{
	float fromLeft = static_cast<float>((flags & AnimationFlag_FromLeft) == AnimationFlag_FromLeft);
	float fromRight = static_cast<float>((flags & AnimationFlag_FromRight) == AnimationFlag_FromRight);
	float fromTop = static_cast<float>((flags & AnimationFlag_FromTop) == AnimationFlag_FromTop);
	float fromBottom = static_cast<float>((flags & AnimationFlag_FromBottom) == AnimationFlag_FromBottom);
	float fade = static_cast<float>((flags & AnimationFlag_Fade) == AnimationFlag_Fade);

	if (nextSrc)
		*nextSrc = vec3(-1.0f * fromLeft + 1.0f * fromRight, 1.0f * fromTop - 1.0f * fromBottom, 1.0f - fade);

	if (nextDst)
		*nextDst = vec3(0.0, 0.0, 1.0f);

	if (currDst)
		*currDst = vec3(1.0f * fromLeft - 1.0f * fromRight, -1.0f * fromTop + 1.0f * fromBottom, 1.0f - fade);
}

void Gui::replaceTopmostLayout(Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	removeLayout(topmostLayout(), animationFlags, duration);
	pushLayout(newLayout, animationFlags, duration);
}

void Gui::popTopmostLayout(size_t animationFlags, float duration)
{
	removeLayout(topmostLayout(), animationFlags, duration);
}

void Gui::replaceLayout(Layout::Pointer oldLayout, Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	LayoutEntryStack::iterator i = _layouts.begin();
	while (i != _layouts.end())
	{
		if ((*i)->layout == oldLayout)
			break;

		++i;
	}

	removeLayout(oldLayout, animationFlags, duration);

	if (i == _layouts.end())
	{
		pushLayout(newLayout, animationFlags, duration);
	}
	else 
	{
		LayoutEntry newEntry(new LayoutEntryObject(this, _rc, newLayout));
		_layouts.insert(i, newEntry);

		animateLayoutAppearing(newLayout, newEntry.ptr(), animationFlags, duration);
	}
}

void Gui::removeLayout(Layout::Pointer oldLayout, size_t animationFlags, float duration)
{
	LayoutEntryObject* entry = entryForLayout(oldLayout);
	if (entry == 0) return;

	layoutWillDisappear.invoke(oldLayout);
	oldLayout->willDisappear();
	oldLayout->layoutDoesntNeedKeyboard.disconnect(this);
	oldLayout->layoutRequiresKeyboard.disconnect(this);

	if ((animationFlags == AnimationFlag_None) || (fabsf(duration) < 0.001f))
	{
		oldLayout->didDisappear();
		removeLayoutFromList(oldLayout);
	}
	else 
	{
		vec3 destOffsetAlpha;
		getAnimationParams(animationFlags, 0, 0, &destOffsetAlpha);
		entry->animateTo(destOffsetAlpha, fabsf(duration), Gui::LayoutEntryObject::State_Disappear);
	}
}

void Gui::pushLayout(Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	if (!newLayout.valid()) return;

	_layouts.push_back(LayoutEntry(new LayoutEntryObject(this, _rc, newLayout)));
	animateLayoutAppearing(newLayout, _layouts.back().ptr(), animationFlags, duration);
}

void Gui::animateLayoutAppearing(Layout::Pointer newLayout, LayoutEntryObject* newEntry, size_t animationFlags, float duration)
{
	newLayout->layout(_screenSize);

	layoutWillAppear.invoke(newLayout);
	newLayout->willAppear();
	newLayout->layoutRequiresKeyboard.connect(this, &Gui::onKeyboardNeeded);
	newLayout->layoutDoesntNeedKeyboard.connect(this, &Gui::onKeyboardResigned);

	if ((animationFlags == AnimationFlag_None) || (fabsf(duration) < 0.001f))
	{
		newLayout->didAppear();
	}
	else 
	{
		vec3 destOffsetAlpha;
		getAnimationParams(animationFlags, &newEntry->offsetAlpha, &destOffsetAlpha, 0);
		newEntry->animateTo(destOffsetAlpha, fabsf(duration), Gui::LayoutEntryObject::State_Appear);
	}
}

void Gui::removeLayoutFromList(Layout::Pointer ptr)
{
	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		if ((*i)->layout == ptr)
		{
			_layouts.erase(i);
			return;
		}
	}
}

void Gui::removeLayoutEntryFromList(LayoutEntryObject* ptr)
{
	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		if (i->ptr() == ptr)
		{
			_layouts.erase(i);
			return;
		}
	}
}

void Gui::layoutEntryTransitionFinished(LayoutEntryObject* l)
{
	if (l->state == LayoutEntryObject::State_Disappear)
	{
		layoutDidDisappear.invoke(l->layout);
		l->layout->didDisappear();

		removeLayoutEntryFromList(l);
	}
	else 
	{
		layoutDidAppear.invoke(l->layout);
		l->layout->didAppear();

		l->state = Gui::LayoutEntryObject::State_Still;
	}
}

bool Gui::hasLayout(Layout::Pointer aLayout)
{
	if (aLayout.invalid()) return false;

	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		if ((*i)->layout == aLayout)
			return true;
	}

	return false;
}

Gui::LayoutEntryObject* Gui::entryForLayout(Layout::Pointer ptr)
{
	if (ptr.invalid()) return 0;

	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		if ((*i)->layout == ptr)
			return i->ptr();
	}

	return 0;
}

bool Gui::animatingTransition()
{
	for (LayoutEntryStack::iterator i = _layouts.begin(), e = _layouts.end(); i != e; ++i)
	{
		if (i->ptr()->state != Gui::LayoutEntryObject::State_Still)
			return true;
	}

	return false;
}

/*
 * Layout Entry
 */

Gui::LayoutEntryObject::LayoutEntryObject(Gui* own, RenderContext* rc, Layout::Pointer l) : 
	owner(own),	renderingElement(new RenderingElement(rc)), layout(l), animator(0), 
	offsetAlpha(0.0f, 0.0f, 1.0f), state(Gui::LayoutEntryObject::State_Still)
{
}

Gui::LayoutEntryObject::LayoutEntryObject(Gui::LayoutEntryObject&& l) : 
	owner(l.owner), renderingElement(l.renderingElement), layout(l.layout), animator(l.animator.extract()), 
	offsetAlpha(l.offsetAlpha), state(Gui::LayoutEntryObject::State_Still)
{
	moveDelegate();
}

Gui::LayoutEntryObject::LayoutEntryObject(Gui::LayoutEntryObject& l) : 
	owner(l.owner), renderingElement(l.renderingElement), layout(l.layout), animator(l.animator.extract()), 
	offsetAlpha(l.offsetAlpha), state(Gui::LayoutEntryObject::State_Still)
{
	moveDelegate();
}


Gui::LayoutEntryObject& Gui::LayoutEntryObject::operator = (Gui::LayoutEntryObject& l)
{
	owner = l.owner;
	renderingElement = l.renderingElement;
	layout = l.layout;
	animator = l.animator.extract();
	offsetAlpha = l.offsetAlpha;
	state = l.state;
	moveDelegate();
	return *this; 
}

void Gui::LayoutEntryObject::moveDelegate()
{
	if (animator.valid())
		animator->setDelegate(0);
}

void Gui::LayoutEntryObject::animateTo(const vec3& oa, float duration, State s)
{
	state = s;

	if (animator.valid())
		animator.extract()->destroy();

	animator = new Vector3Animator(this, &offsetAlpha, offsetAlpha, oa, duration, 0, mainTimerPool()); 
}

void Gui::LayoutEntryObject::animatorUpdated(BaseAnimator*)
{
}

void Gui::LayoutEntryObject::animatorFinished(BaseAnimator* a)
{
	animator.extract()->destroy();
	owner->layoutEntryTransitionFinished(this);
}

void Gui::showMessageView(MessageView::Pointer mv)
{
	mv->messageViewButtonSelected.connect(this, &Gui::onMessageViewButtonClicked);
	pushLayout(mv);
}

void Gui::onMessageViewButtonClicked(MessageView* view, MessageViewButton button)
{
	removeLayout(Layout::Pointer(view));
}