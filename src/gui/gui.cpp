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


Gui::Gui(RenderContext* rc, TextureCache& texCache) : _rc(rc), _textureCache(texCache), _renderer(rc, true), 
	_renderingElementBackground(new RenderingElement(rc)),
#if (ET_ENABLE_CUSTOM_KEYBOARD)
	_renderingElementKeyboard(new RenderingElement(rc)),
	_keyboard(rc, Font(CharacterGenerator::Pointer(new CharacterGenerator(rc, "System", "System", 14))), _textureCache),
#endif
	_background(Texture(), 0), _backgroundValid(true)
{
	_background.setPivotPoint(vec2(0.5f));
	_background.setContentMode(ImageView::ContentMode_Stretch);
	layout(rc->size());
}

bool Gui::pointerPressed(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

#if (ET_ENABLE_CUSTOM_KEYBOARD)
	bool processed = _keyboard.visible() && _keyboard.pointerPressed(p);
#else
	bool processed = false;
#endif
	
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

#if (ET_ENABLE_CUSTOM_KEYBOARD)
	bool processed = _keyboard.visible() && _keyboard.pointerMoved(p);
#else
	bool processed = false;
#endif

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

#if (ET_ENABLE_CUSTOM_KEYBOARD)
	bool processed = _keyboard.visible() && _keyboard.pointerReleased(p);
#else
	bool processed = false;
#endif
	
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

bool Gui::pointerCancelled(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;
	
#if (ET_ENABLE_CUSTOM_KEYBOARD)
	bool processed = _keyboard.visible() && _keyboard.pointerCancelled(p);
#else
	bool processed = false;
#endif
	
	if (!processed)
	{
		for (LayoutEntryStack::reverse_iterator i = _layouts.rbegin(), e = _layouts.rend(); i != e; ++i)
		{
			if ((*i)->layout->pointerCancelled(p))
				return true;
		}
	}
	
	return processed;
}

bool Gui::pointerScrolled(const et::PointerInputInfo& p)
{
	if (animatingTransition()) return true;

#if (ET_ENABLE_CUSTOM_KEYBOARD)
	bool processed = _keyboard.visible() && _keyboard.pointerScrolled(p);
#else
	bool processed = false;
#endif
	
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

void Gui::buildLayoutVertices(RenderContext* rc, RenderingElement::Pointer element, Layout::Pointer layout)
{
	_renderer.setRendernigElement(element);

	if (!layout->valid())
	{
		element->clear();
		layout->addToRenderQueue(rc, _renderer);
	}
}

#if (ET_ENABLE_CUSTOM_KEYBOARD)
void Gui::buildKeyboardVertices(RenderContext* rc)
{
	if (_keyboard.invalid())
	{
		_renderingElementKeyboard->clear();
		_keyboard.addToRenderQueue(rc, _renderer);
	}
}
#else
void Gui::buildKeyboardVertices(RenderContext*) { }
#endif

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
	
#if (ET_ENABLE_CUSTOM_KEYBOARD)
	_keyboard.layout(size);
#endif
	
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
		buildLayoutVertices(rc, obj->layout->renderingElement(), obj->layout);
		_renderer.setCustomAlpha(obj->offsetAlpha.z);
		_renderer.setCustomOffset(obj->offsetAlpha.xy());
		_renderer.render(rc);
	}

#if (ET_ENABLE_CUSTOM_KEYBOARD)
	if (_keyboard.visible())
	{
		_renderer.setRendernigElement(_renderingElementKeyboard);
		buildKeyboardVertices(rc);
		_renderer.setCustomAlpha(1.0f);
		_renderer.setCustomOffset(vec2(0.0f));
		_renderer.render(rc);
	}
#endif
	
	_renderer.endRender(rc);
}

void Gui::setBackgroundImage(const Image& img)
{
	_background.setImage(img);
	_backgroundValid = false;
}

#if (ET_ENABLE_CUSTOM_KEYBOARD)
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
#else
void Gui::onKeyboardNeeded(Layout*, Element*) { }
#endif

#if (ET_ENABLE_CUSTOM_KEYBOARD)
void Gui::onKeyboardResigned(Layout* l)
{
	if (!platformHasHardwareKeyboard() && _keyboard.visible())
		l->resetVerticalOffset();
	_keyboard.hide(true);
}
#else
void Gui::onKeyboardResigned(Layout*) { }
#endif

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

void Gui::internal_replaceTopmostLayout(Layout::Pointer newLayout, AnimationDescriptor desc)
{
	removeLayout(topmostLayout(), desc.flags, desc.duration);
	pushLayout(newLayout, desc.flags, desc.duration);
}

void Gui::internal_replaceLayout(LayoutPair l, AnimationDescriptor desc)
{
	LayoutEntryStack::iterator i = _layouts.begin();
	while (i != _layouts.end())
	{
		if ((*i)->layout == l.oldLayout)
			break;

		++i;
	}

	removeLayout(l.oldLayout, desc.flags, desc.duration);

	if (i == _layouts.end())
	{
		pushLayout(l.newLayout, desc.flags, desc.duration);
	}
	else 
	{
		LayoutEntry newEntry(new LayoutEntryObject(this, _rc, l.newLayout));
		_layouts.insert(i, newEntry);

		animateLayoutAppearing(l.newLayout, newEntry.ptr(), desc.flags, desc.duration);
	}
}

void Gui::internal_removeLayout(Layout::Pointer oldLayout, AnimationDescriptor desc)
{
	LayoutEntryObject* entry = entryForLayout(oldLayout);
	if (entry == 0) return;

	layoutWillDisappear.invoke(oldLayout);
	oldLayout->willDisappear();
	oldLayout->layoutDoesntNeedKeyboard.disconnect(this);
	oldLayout->layoutRequiresKeyboard.disconnect(this);

	if ((desc.flags == AnimationFlag_None) || (fabsf(desc.duration) < std::numeric_limits<float>::epsilon()))
	{
		oldLayout->didDisappear();
		removeLayoutFromList(oldLayout);
	}
	else 
	{
		vec3 destOffsetAlpha;
		getAnimationParams(desc.flags, 0, 0, &destOffsetAlpha);
		entry->animateTo(destOffsetAlpha, fabsf(desc.duration), Gui::LayoutEntryObject::State_Disappear);
	}
}

void Gui::internal_pushLayout(Layout::Pointer newLayout, AnimationDescriptor desc)
{
	if (newLayout.invalid()) return;

	if (hasLayout(newLayout))
		internal_removeLayout(newLayout, AnimationDescriptor());

	_layouts.push_back(LayoutEntry(new LayoutEntryObject(this, _rc, newLayout)));
	animateLayoutAppearing(newLayout, _layouts.back().ptr(), desc.flags, desc.duration);
}

void Gui::animateLayoutAppearing(Layout::Pointer newLayout, LayoutEntryObject* newEntry, size_t animationFlags, float duration)
{
	newLayout->layout(_screenSize);

	layoutWillAppear.invoke(newLayout);
	newLayout->willAppear();
	newLayout->layoutRequiresKeyboard.connect(this, &Gui::onKeyboardNeeded);
	newLayout->layoutDoesntNeedKeyboard.connect(this, &Gui::onKeyboardResigned);

	bool smallDuration = std::abs(duration) < std::numeric_limits<float>::epsilon();
	if ((animationFlags == AnimationFlag_None) || smallDuration)
	{
		newLayout->didAppear();
		layoutDidAppear.invoke(newLayout);
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
	owner(own),	layout(l), animator(0), 
	offsetAlpha(0.0f, 0.0f, 1.0f), state(Gui::LayoutEntryObject::State_Still)
{
	l->initRenderingElement(rc);
}

Gui::LayoutEntryObject::LayoutEntryObject(Gui::LayoutEntryObject&& l) : 
	owner(l.owner), layout(l.layout), animator(l.animator.extract()), 
	offsetAlpha(l.offsetAlpha), state(Gui::LayoutEntryObject::State_Still)
{
	moveDelegate();
}

Gui::LayoutEntryObject::LayoutEntryObject(Gui::LayoutEntryObject& l) : 
	owner(l.owner), layout(l.layout), animator(l.animator.extract()), 
	offsetAlpha(l.offsetAlpha), state(Gui::LayoutEntryObject::State_Still)
{
	moveDelegate();
}

Gui::LayoutEntryObject& Gui::LayoutEntryObject::operator = (Gui::LayoutEntryObject& l)
{
	owner = l.owner;
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

void Gui::LayoutEntryObject::animatorFinished(BaseAnimator*)
{
	animator.extract()->destroy();
	owner->layoutEntryTransitionFinished(this);
}

void Gui::showMessageView(MessageView::Pointer mv, size_t animationFlags, float duration)
{
	mv->messageViewButtonSelected.connect(this, &Gui::onMessageViewButtonClicked);
	pushLayout(mv, animationFlags, duration);
}

void Gui::onMessageViewButtonClicked(MessageView* view, MessageViewButton)
{
	removeLayout(Layout::Pointer(view));
}

void Gui::replaceTopmostLayout(Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	ET_INVOKE_THIS_CLASS_METHOD2(Gui, internal_replaceTopmostLayout, newLayout, AnimationDescriptor(animationFlags, duration))
}

void Gui::popTopmostLayout(size_t animationFlags, float duration)
{
	ET_INVOKE_THIS_CLASS_METHOD2(Gui, internal_removeLayout, topmostLayout(), AnimationDescriptor(animationFlags, duration))
}

void Gui::replaceLayout(Layout::Pointer oldLayout, Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	ET_INVOKE_THIS_CLASS_METHOD2(Gui, internal_replaceLayout, LayoutPair(oldLayout, newLayout), AnimationDescriptor(animationFlags, duration))
}

void Gui::removeLayout(Layout::Pointer oldLayout, size_t animationFlags, float duration)
{
	ET_INVOKE_THIS_CLASS_METHOD2(Gui, internal_removeLayout, oldLayout, AnimationDescriptor(animationFlags, duration))
}

void Gui::pushLayout(Layout::Pointer newLayout, size_t animationFlags, float duration)
{
	ET_INVOKE_THIS_CLASS_METHOD2(Gui, internal_pushLayout, newLayout, AnimationDescriptor(animationFlags, duration))
}

