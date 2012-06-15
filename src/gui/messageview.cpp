/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/messageview.h>

using namespace et;
using namespace et::gui;

MessageView::MessageView(const std::string& text, Font font, const Image& image, size_t buttons,
						 const std::string& button1title, const std::string& button2title) : _buttonFlags(buttons)
{
	_background = ImageView::Pointer(new ImageView(Texture(), this));
	_background->setPivotPoint(vec2(0.5f));
	
	_image = ImageView::Pointer(new ImageView(image, _background.ptr()));
	
	_text = Label::Pointer(new Label(text, font, _background.ptr()));
	_text->setPivotPoint(vec2(0.5f, 0.0f));
	_text->setHorizontalAlignment(ElementAlignment_Center);
	
	_button1 = Button::Pointer(new Button(button1title, font, _background.ptr()));
	_button1->clicked.connect(this, &MessageView::buttonClicked);
	_button1->setPivotPoint(hasSecondButton() ? vec2(0.0f, 1.0f) : vec2(0.5f, 1.0f));
	
	_button2 = Button::Pointer(new Button(button2title, font, _background.ptr()));
	_button2->clicked.connect(this, &MessageView::buttonClicked);
	_button2->setPivotPoint(vec2(1.0f, 1.0f));
	_button2->setVisible(hasSecondButton());
}

void MessageView::layout(const vec2& sz)
{
	vec2 bgSize = maxv(_background->imageDescriptor().size, floorv(vec2(0.9f * sz.x, 0.35f * sz.y)));
	_background->setFrame(floorv(0.5f * sz), bgSize);

	vec2 bOffset = floorv(0.15f * bgSize);
	vec2 b1pos = hasSecondButton() ? vec2(bOffset.x, bgSize.y - bOffset.y) : vec2(0.5f * bgSize.x, bgSize.y - bOffset.y);
	vec2 b2pos = bgSize - bOffset;
	
	vec2 maxButtonSize = maxv(_button1->size(), _button2->size());
	_button1->setFrame(floorv(b1pos), maxButtonSize);
	_button2->setFrame(floorv(b2pos), maxButtonSize);
	
	float topOrigin = etMin(_button1->origin().y, _button2->origin().y);
	
	vec2 textPos = vec2(0.5f * bgSize.x, 0.5f * (bgSize.y - topOrigin));
	_text->setPosition(floorv(textPos));
}

void MessageView::setBackgroundImage(const Image& img)
{
	_background->setImage(img);
}

void MessageView::setButtonBackground(const Image& img, ElementState s)
{
	_button1->setBackgroundForState(img, s);
	_button2->setBackgroundForState(img, s);
	
	_button1->adjustSize();
	_button2->adjustSize();
}

void MessageView::setButtonTextColor(const vec4& color)
{
	_button1->setTextColor(color);
	_button2->setTextColor(color);
}

void MessageView::setButtonPressedTextColor(const vec4& color)
{
	_button1->setTextPressedColor(color);
	_button2->setTextPressedColor(color);
}

bool MessageView::hasSecondButton() const
{
	return (_buttonFlags & MessageViewButton_Second) == MessageViewButton_Second;
}

void MessageView::buttonClicked(Button* btn)
{
	if (btn == _button1.ptr())
	{
		messageViewButtonSelected.invokeInMainRunLoop(this, MessageViewButton_First);
	}
	else if (btn == _button2.ptr())
	{
		messageViewButtonSelected.invokeInMainRunLoop(this, MessageViewButton_Second);
	}
	else
	{
		messageViewButtonSelected.invokeInMainRunLoop(this, MessageViewButton_Any);
	}
}