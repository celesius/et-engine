/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/gui/Element2d.h>
#include <et/gui/button.h>

namespace et
{
	namespace gui
	{
		class Keyboard;

		class KeyboardDelegate
		{
		public:
			virtual void keyboardDidReturnInElement(Keyboard*, Element*) { /* virtual */ };
		};

#if (ET_ENABLE_CUSTOM_KEYBOARD)
		class Keyboard : public Element2d
		{
		public:
			Keyboard(RenderContext* rc, Font font, TextureCache& cache);
			~Keyboard();

			void layout(const vec2& sz);
			void addToRenderQueue(RenderContext*, GuiRenderer&);

			inline bool invalid() const
				{ return _invalid; }

			inline bool visible() const
				{ return _visible; }

			void show(bool animated, Element* forElement);
			void hide(bool animated);

			void setDelegate(KeyboardDelegate* delegate);
			void setInvalid();

			bool pointerPressed(const PointerInputInfo&);
			bool pointerMoved(const PointerInputInfo&);
			bool pointerReleased(const PointerInputInfo&);
			bool pointerScrolled(const PointerInputInfo&);

			float topOrigin() const;

		private:
			typedef std::vector<Button::Pointer> ButtonList;

			Button::Pointer createButton(ButtonList& list, const std::string& title, Font f, bool accented, const vec2& pivot = vec2(0.0f));
			void genServiceButtons(Font font, float lastRowWidth);
			float genPrimaryButtons(Font font);
			float genExtraButtons(Font font);

			void switchKeyboards(bool animated);
			void setActiveButton(Button::Pointer btn, const PointerInputInfo& np);

			void onButtonPressed(Button* b);
			void onShiftPressed(Button* b);
			void onDeletePressed(Button* b);
			void onSpacePressed(Button* b);
			void onReturnPressed(Button* b);
			void onSwitchKeyboardsPressed(Button* b);

			void onKeyPressed(unsigned char c);
			void onKeyReleased(unsigned char c);
			void onCharEntered(unsigned char c);

			void performButtonAction(unsigned char c);

		private:
			KeyboardDelegate* _delegate;
			Texture _texture;
			GuiVertexList _vertices;
			ImageDescriptor _descriptor;
			ButtonList _primaryButtons;
			ButtonList _extraButtons;
			ButtonList _serviceButtons;
			Button::Pointer _lastActiveButton;
			Button::Pointer _pressedButton;
			Element* _capturedElement;
			vec2 _screenSize;
			bool _extraButtonsVisible;
			bool _invalid;
			bool _visible;
			bool _shift;
			bool _shiftLocked;
		};
#endif
	}
}

