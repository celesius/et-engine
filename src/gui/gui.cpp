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

Gui::Gui(RenderContext* rc, TextureCache& texCache) : _textureCache(texCache), _renderer(rc), 
	_renderingElementBackground(new RenderingElement(rc, "background-rendering-element")), 
	_renderingElementCurrentLayout(new RenderingElement(rc, "current-layout-rendering-element")), 
	_renderingElementNextLayout(new RenderingElement(rc, "next-layout-rendering-element")), 
	_renderingElementCurrentModalLayout(new RenderingElement(rc, "current-modal-layout-rendering-element")),
	_renderingElementNextModalLayout(new RenderingElement(rc, "next-modal-layout-rendering-element")),
	_renderingElementKeyboard(new RenderingElement(rc, "keyboard-rendering-element")),
	_keyboard(rc, Font(rc, "ui/fonts/main.font", _textureCache), texCache), 
	_currentLayout(0), _nextLayout(0), _currentModalLayout(0), _nextModalLayout(0),
	_currentLayoutAnimator(0), _nextLayoutAnimator(0), _currentModalLayoutAnimator(0), _nextModalLayoutAnimator(0),
	_currentLayoutOffsetAlpha(0.0f, 0.0f, 1.0f),
	_currentModalLayoutOffsetAlpha(0.0f, 0.0f, 1.0f),
	_background(Texture(), 0), _backgroundValid(true), _switchingRegular(false), _switchingModal(false)
{
	_background.setPivotPoint(vec2(0.5f));
	_background.setContentMode(ImageView::ContentMode_Stretch);
	
	layout(rc->size());
}

bool Gui::pointerPressed(const et::PointerInputInfo& p)
{
	if (_switchingRegular || _switchingModal) return true;

	bool processed = false;

	if (_keyboard.visible())
		processed = _keyboard.pointerPressed(p);

	if (!processed && _currentModalLayout.valid())
		processed = _currentModalLayout->pointerPressed(p);

	if (!processed && _currentLayout.valid())
		processed = _currentLayout->pointerPressed(p);

	return processed;
}

bool Gui::pointerMoved(const et::PointerInputInfo& p)
{
	if (_switchingRegular || _switchingModal) return true;

	bool processed = false;

	if (_keyboard.visible())
		processed = _keyboard.pointerMoved(p);

	if (!processed && _currentModalLayout.valid())
		processed = _currentModalLayout->pointerMoved(p);

	if (!processed && _currentLayout.valid())
		processed = _currentLayout->pointerMoved(p);

	return processed;
}

bool Gui::pointerReleased(const et::PointerInputInfo& p)
{
	if (_switchingRegular || _switchingModal) return true;

	bool processed = false;

	if (_keyboard.visible())
		processed = _keyboard.pointerReleased(p);

	if (!processed && _currentModalLayout.valid())
		processed = _currentModalLayout->pointerReleased(p);

	if (!processed && _currentLayout.valid())
		processed = _currentLayout->pointerReleased(p);

	return processed;
}

bool Gui::pointerScrolled(const et::PointerInputInfo& p)
{
	if (_switchingRegular || _switchingModal) return true;

	bool processed = false;

	if (_keyboard.visible())
		processed = _keyboard.pointerScrolled(p);

	if (!processed && _currentModalLayout.valid())
		processed = _currentModalLayout->pointerScrolled(p);

	if (!processed && _currentLayout.valid())
		processed = _currentLayout->pointerScrolled(p);

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

	if (_currentLayout.valid())
		_currentLayout->layout(size);
	if (_nextLayout.valid())
		_nextLayout->layout(size);
	if (_currentModalLayout.valid())
		_currentModalLayout->layout(size);
	if (_nextModalLayout.valid())
		_nextModalLayout->layout(size);
}

void Gui::render(RenderContext* rc)
{
	_renderer.beginRender(rc);

	bool hasBackground = _background.texture().valid();
	if (hasBackground)
	{
		_renderer.setRendernigElement(_renderingElementBackground);
		buildBackgroundVertices(rc);
		_renderer.setCustomAlpha(1.0f);
		_renderer.setCustomOffset(vec2(0.0f));
		_renderer.render(rc);
	}

	bool hasCurrentLayout = _currentLayout.valid();
	if (hasCurrentLayout)
	{
		buildLayoutVertices(rc, _renderingElementCurrentLayout, _currentLayout);
		_renderer.setCustomAlpha(_currentLayoutOffsetAlpha.z);
		_renderer.setCustomOffset(_currentLayoutOffsetAlpha.xy());
		_renderer.render(rc);
	}

	bool hasNextLayout = _nextLayout.valid();
	if (hasNextLayout)
	{
		buildLayoutVertices(rc, _renderingElementNextLayout, _nextLayout);
		_renderer.setCustomAlpha(_nextLayoutOffsetAlpha.z);
		_renderer.setCustomOffset(_nextLayoutOffsetAlpha.xy());
		_renderer.render(rc);
	}

	bool hasCurrentModalLayout = _currentModalLayout.valid();
	if (hasCurrentModalLayout)
	{
		buildLayoutVertices(rc, _renderingElementCurrentModalLayout, _currentModalLayout);
		_renderer.setCustomAlpha(_currentModalLayoutOffsetAlpha.z);
		_renderer.setCustomOffset(_currentModalLayoutOffsetAlpha.xy());
		_renderer.render(rc);
	}

	bool hasNextModalLayout = _nextModalLayout.valid();
	if (hasNextModalLayout)
	{
		buildLayoutVertices(rc, _renderingElementNextModalLayout, _nextModalLayout);
		_renderer.setCustomAlpha(_nextModalLayoutOffsetAlpha.z);
		_renderer.setCustomOffset(_nextModalLayoutOffsetAlpha.xy());
		_renderer.render(rc);
	}

	bool hasKeyboard = _keyboard.visible();
	if (hasKeyboard)
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

void Gui::setCurrentLayout(Layout::Pointer layout, size_t animationFlags, float duration)
{
	if (_currentLayout == layout) return;

	if ((duration == 0.0f) || animationFlags == AnimationFlag_None)
	{
		if (_currentLayout.valid())
		{
			_currentLayout->layoutDoesntNeedKeyboard.disconnect(this);
			_currentLayout->layoutRequiresKeyboard.disconnect(this);
			_currentLayout->willDisappear();
			_currentLayout->didDisappear();
		}

		_nextLayout.reset(0);
		_currentLayout = layout;

		if (_currentLayout.valid())
		{
			_currentLayout->layoutRequiresKeyboard.connect(this, &Gui::onKeyboardNeeded);
			_currentLayout->layoutDoesntNeedKeyboard.connect(this, &Gui::onKeyboardResigned);
			_currentLayout->layout(_screenSize);
			_currentLayout->willAppear();
			_currentLayout->didAppear();
		}

		layoutDidAppear.invoke(_currentLayout);
	}
	else
	{
		_switchingRegular = true;
		_nextLayout = layout;

		if (_currentLayoutAnimator)
		{
			_currentLayoutAnimator->destroy();
			_currentLayoutAnimator = 0;
		}

		if (_nextLayoutAnimator)
		{
			_nextLayoutAnimator->destroy();
			_nextLayoutAnimator = 0;
		}

		vec3 nextDst;
		vec3 currDst;
		getAnimationParams(animationFlags, &_nextLayoutOffsetAlpha, &nextDst, &currDst);

		if (_currentLayout.valid())
		{
			_currentLayout->layoutDoesntNeedKeyboard.disconnect(this);
			_currentLayout->layoutRequiresKeyboard.disconnect(this);
			_currentLayout->willDisappear();

			layoutWillDisappear.invoke(_currentLayout);
			_currentLayoutAnimator = new Vector3Animator(this, &_currentLayoutOffsetAlpha, _currentLayoutOffsetAlpha, currDst, 
				duration, 0, mainTimerPool());
		}

		if (_nextLayout.valid())
		{
			_nextLayout->layoutRequiresKeyboard.connect(this, &Gui::onKeyboardNeeded);
			_nextLayout->layoutDoesntNeedKeyboard.connect(this, &Gui::onKeyboardResigned);
			_nextLayout->layout(_screenSize);
			_nextLayout->willAppear();

			layoutWillAppear.invoke(_nextLayout);
			_nextLayoutAnimator = new Vector3Animator(this, &_nextLayoutOffsetAlpha, _nextLayoutOffsetAlpha, nextDst, 
				duration, 0, mainTimerPool());
		}
	}
}

void Gui::presentModalLayout(Layout::Pointer modalLayout, size_t animationFlags, float duration)
{
	if (modalLayout == _currentModalLayout) return;

	if ((duration == 0.0f) || (animationFlags == AnimationFlag_None))
	{
		if (_currentModalLayout.valid())
		{
			_currentModalLayout->willDisappear();
			_currentModalLayout->didDisappear();
		}

		_nextModalLayout.reset(0);
		_currentModalLayout = modalLayout;

		if (_currentModalLayout.valid())
		{
			_currentModalLayout->layout(_screenSize);
			_currentModalLayout->willAppear();
			_currentModalLayout->didAppear();
		}

		layoutDidAppear.invoke(_currentModalLayout);
	}
	else
	{
		_switchingModal = true;
		_nextModalLayout = modalLayout;

		if (_currentModalLayoutAnimator)
		{
			_currentModalLayoutAnimator->destroy();
			_currentModalLayoutAnimator = 0;
		}

		if (_nextModalLayoutAnimator)
		{
			_nextModalLayoutAnimator->destroy();
			_nextModalLayoutAnimator = 0;
		}

		vec3 nextDst;
		vec3 currDst;
		getAnimationParams(animationFlags, &_nextModalLayoutOffsetAlpha, &nextDst, &currDst);

		if (_currentModalLayout.valid())
		{
			layoutWillDisappear.invoke(_currentModalLayout);
			_currentModalLayout->willDisappear();
			_currentModalLayout->invalidateContent();
			_currentModalLayoutAnimator = new Vector3Animator(this, &_currentModalLayoutOffsetAlpha, 
				_currentModalLayoutOffsetAlpha, currDst, duration, 0, mainTimerPool());
		}

		if (_nextModalLayout.valid())
		{
			_nextModalLayout->layout(_screenSize);
			layoutWillAppear.invoke(_nextModalLayout);
			_nextModalLayout->willAppear();
			_nextModalLayout->invalidateContent();
			_nextModalLayoutAnimator = new Vector3Animator(this, &_nextModalLayoutOffsetAlpha, 
				_nextModalLayoutOffsetAlpha, nextDst, duration, 0, mainTimerPool());
		}
	}
}

void Gui::dismissModalLayout(size_t animationFlags, float duration)
{
	presentModalLayout(Layout::Pointer(), animationFlags, duration);
}

void Gui::animatorUpdated(BaseAnimator*)
{

}

void Gui::exchangeLayouts(Layout::Pointer& c, Layout::Pointer& n)
{
	layoutDidDisappear.invoke(c);
	layoutDidAppear.invoke(n);

	if (c.valid())
	{
		c->layoutRequiresKeyboard.disconnect(this);
		c->layoutDoesntNeedKeyboard.disconnect(this);
		c->didDisappear();
	}

	c = n;
	n.reset(0);

	if (c.valid())
	{
		c->layoutRequiresKeyboard.connect(this, &Gui::onKeyboardNeeded);
		c->layoutDoesntNeedKeyboard.connect(this, &Gui::onKeyboardResigned);
		c->didAppear();
		c->invalidateContent();
	}
}

void Gui::processAnimatorFinishForRegularLayout(BaseAnimator* a)
{
	bool hideAnimatorsChanged = false;
	if (a == _currentLayoutAnimator)
	{
		hideAnimatorsChanged = true;
		_currentLayoutAnimator = 0;
	}
	if (a == _nextLayoutAnimator)
	{
		hideAnimatorsChanged = true;
		_nextLayoutAnimator = 0;
	}

	if (hideAnimatorsChanged && (_nextLayoutAnimator == 0) && (_currentLayoutAnimator == 0))
	{
		_switchingRegular = false;
		_currentLayoutOffsetAlpha = _nextLayoutOffsetAlpha;
		exchangeLayouts(_currentLayout, _nextLayout);
	}
}

void Gui::processAnimatorFinishForModalLayout(BaseAnimator* a)
{
	bool hideAnimatorsChanged = false;
	if (a == _currentModalLayoutAnimator)
	{
		hideAnimatorsChanged = true;
		_currentModalLayoutAnimator = 0;
	}
	if (a == _nextModalLayoutAnimator)
	{
		hideAnimatorsChanged = true;
		_nextModalLayoutAnimator = 0;
	}

	if (hideAnimatorsChanged && (_nextModalLayoutAnimator == 0) && (_currentModalLayoutAnimator == 0))
	{
		_switchingModal = false;
		_currentModalLayoutOffsetAlpha = _nextModalLayoutOffsetAlpha;
		exchangeLayouts(_currentModalLayout, _nextModalLayout);
	}
}

void Gui::animatorFinished(BaseAnimator* a)
{
	processAnimatorFinishForModalLayout(a);
	processAnimatorFinishForRegularLayout(a);
	a->destroy();
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
	*nextSrc = vec3(0.0, 0.0, 1.0f);
	*nextDst = vec3(0.0, 0.0, 1.0f);
	*currDst = vec3(0.0, 0.0, 1.0f);

	float fromLeft = static_cast<float>((flags & AnimationFlag_FromLeft) == AnimationFlag_FromLeft);
	float fromRight = static_cast<float>((flags & AnimationFlag_FromRight) == AnimationFlag_FromRight);
	float fromTop = static_cast<float>((flags & AnimationFlag_FromTop) == AnimationFlag_FromTop);
	float fromBottom = static_cast<float>((flags & AnimationFlag_FromBottom) == AnimationFlag_FromBottom);

	nextSrc->x = -1.0f * fromLeft + 1.0f * fromRight;
	currDst->x = 1.0f * fromLeft - 1.0f * fromRight;
	nextSrc->y = 1.0f * fromTop - 1.0f * fromBottom;
	currDst->y = -1.0f * fromTop + 1.0f * fromBottom;

	if ((flags & AnimationFlag_Fade) == AnimationFlag_Fade)
	{
		nextSrc->z = 0.0f;
		nextDst->z = 1.0f;
		currDst->z = 0.0f;
	}
}