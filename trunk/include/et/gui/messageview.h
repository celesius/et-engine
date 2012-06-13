/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/gui/layout.h>
#include <et/gui/imageview.h>
#include <et/gui/label.h>
#include <et/gui/button.h>

namespace et
{
	namespace gui
	{
		enum MessageViewButton
		{
			MessageViewButton_First = 0x00,
			MessageViewButton_Second = 0x01,
			MessageViewButton_Any = 0xf0,
		};
		
		class MessageView : public ModalLayout
		{
		public:
			typedef IntrusivePtr<MessageView> Pointer;

		public:
			MessageView(const std::string& text, Font font, const Image& image = Image(), size_t buttons = MessageViewButton_First,
						const std::string& button1title = "Close", const std::string& button2title = "Ok");

			void setBackgroundImage(const Image& img);
			void layout(const vec2&);
			
			void setButtonBackground(const Image& img, ElementState s);
			void setButtonTextColor(const vec4& color);
			void setButtonPressedTextColor(const vec4& color);
			
			ET_DECLARE_EVENT2(messageViewButtonSelected, MessageView*, MessageViewButton)
			
		private:
			void buttonClicked(Button* btn);
			bool hasSecondButton() const;
			
		private:
			size_t _buttonFlags;
			ImageView::Pointer _background;
			ImageView::Pointer _image;
			Label::Pointer _text;
			Button::Pointer _button1;
			Button::Pointer _button2;
		};
	}
}