/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/rendering/rendercontext.h>
#include <et/input/input.h>
#include <et/gui/guirenderer.h>
#include <et/gui/layout.h>
#include <et/gui/fullscreenelement.h>
#include <et/gui/keyboard.h>
#include <et/gui/imageview.h>
#include <et/gui/label.h>
#include <et/gui/button.h>
#include <et/gui/listbox.h>
#include <et/gui/textfield.h>

namespace et
{
	namespace gui
	{
		enum AnimationFlags
		{
			AnimationFlag_None = 0x0,
			AnimationFlag_Fade = 0x1,
			AnimationFlag_FromLeft = 0x2,
			AnimationFlag_FromRight = 0x4,
			AnimationFlag_FromTop = 0x8,
			AnimationFlag_FromBottom = 0x10
		};

		class Gui : public Shared, public EventReceiver, public AnimatorDelegate
		{
		public:
			Gui(RenderContext* rc, TextureCache& texCache);

			void layout(const vec2& size);
			void render(RenderContext* rc);

			GuiRenderer& renderer() 
				{ return _renderer; }

			Layout::Pointer currentLayout() const
				{ return _currentLayout; }

			Layout::Pointer currentModalLayout() const
				{ return _currentModalLayout; }

			void setCurrentLayout(Layout::Pointer layout, size_t animationFlags = AnimationFlag_None, float duration = 0.3f);

			void presentModalLayout(Layout::Pointer layout, size_t animationFlags = AnimationFlag_None, float duration = 0.3f);
			void dismissModalLayout(size_t animationFlags = AnimationFlag_None, float duration = 0.3f);

			void setBackgroundImage(const Image& img);

			bool pointerPressed(const et::PointerInputInfo&);
			bool pointerMoved(const et::PointerInputInfo&);
			bool pointerReleased(const et::PointerInputInfo&);
			bool pointerScrolled(const et::PointerInputInfo&);

			ET_DECLARE_EVENT1(layoutDidAppear, Layout::Pointer)
			ET_DECLARE_EVENT1(layoutDidDisappear, Layout::Pointer)
			ET_DECLARE_EVENT1(layoutWillAppear, Layout::Pointer)
			ET_DECLARE_EVENT1(layoutWillDisappear, Layout::Pointer)

		private:
			void buildLayoutVertices(RenderContext* rc, RenderingElementRef& element, Layout::Pointer layout);
			void buildBackgroundVertices(RenderContext* rc);
			void buildKeyboardVertices(RenderContext* rc);
			void animatorUpdated(BaseAnimator*);
			void animatorFinished(BaseAnimator*);

			void onKeyboardNeeded(Layout* l, Element* e);
			void onKeyboardResigned(Layout* l);

			void getAnimationParams(size_t flags, vec3* nextSrc, vec3* nextDst, vec3* currDst);
			void processAnimatorFinishForRegularLayout(BaseAnimator* a);
			void processAnimatorFinishForModalLayout(BaseAnimator* a);
			void exchangeLayouts(Layout::Pointer& l1, Layout::Pointer& l2);

		private:
			TextureCache& _textureCache;
			GuiRenderer _renderer;
			RenderingElementRef _renderingElementBackground;
			RenderingElementRef _renderingElementCurrentLayout;
			RenderingElementRef _renderingElementNextLayout;
			RenderingElementRef _renderingElementCurrentModalLayout;
			RenderingElementRef _renderingElementNextModalLayout;
			RenderingElementRef _renderingElementKeyboard;

			Keyboard _keyboard;

			Layout::Pointer _currentLayout;
			Layout::Pointer _nextLayout;
			Layout::Pointer _currentModalLayout;
			Layout::Pointer _nextModalLayout;

			BaseAnimator* _currentLayoutAnimator;
			BaseAnimator* _nextLayoutAnimator;
			BaseAnimator* _currentModalLayoutAnimator;
			BaseAnimator* _nextModalLayoutAnimator;

			vec2 _screenSize;
			vec3 _currentLayoutOffsetAlpha;
			vec3 _nextLayoutOffsetAlpha;
			vec3 _currentModalLayoutOffsetAlpha;
			vec3 _nextModalLayoutOffsetAlpha;

			ImageView _background;
			bool _backgroundValid;
			bool _switchingRegular;
			bool _switchingModal;
		};

		typedef IntrusivePtr<Gui> GuiRef;
	}
}
