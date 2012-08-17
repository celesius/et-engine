/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/geometry/geometry.h>
#include <et/gui/messageview.h>

using namespace et;
using namespace et::gui;

MessageView::MessageView(const std::string& text, Font font, const Image& image, size_t buttons,
						 const std::string& button1title, const std::string& button2title) : _buttonFlags(buttons)
{
	_fade = ImageView::Pointer(new ImageView(Texture(), this));
	_fade->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 0.25f));
	
	_background = ImageView::Pointer(new ImageView(Texture(), _fade.ptr()));
	_background->setPivotPoint(vec2(0.5f));
	
	_imgBackground = ImageView::Pointer(new ImageView(image, _background.ptr()));
	_imgImage = ImageView::Pointer(new ImageView(Texture(), _background.ptr()));
	_imgImage->setPivotPoint(vec2(0.5f, 0.0f));
	_imgImage->setContentMode(ImageView::ContentMode_Fit);
	
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
	_fade->setFrame(vec2(0.0f), sz);
	
	float contentWidth = 0.9f * maxv(_background->imageDescriptor().size, 0.95f * sz).x;
	if (_text->textSize().x > contentWidth)
		_text->fitToWidth(contentWidth);
	
	vec2 buttonsSize = 3.5f * maxv(_button1->size(), _button2->size());
	
	float dataHeight = etMax(_text->textSize().y, _imgImage->imageDescriptor().size.y);
	float contentHeight = etMin(dataHeight + buttonsSize.y, sz.y - buttonsSize.y);
	
	vec2 bgSize = maxv(_background->imageDescriptor().size, floorv(vec2(0.95f * sz.x, contentHeight)));
	
	_background->setFrame(floorv(0.5f * sz), bgSize);
	
	vec2 bOffset = floorv(0.1f * bgSize);
	vec2 b1pos = hasSecondButton() ? vec2(bOffset.x, bgSize.y - bOffset.y) : vec2(0.5f * bgSize.x, bgSize.y - bOffset.y);
	vec2 b2pos = bgSize - bOffset;
	
	vec2 maxButtonSize = maxv(_button1->size(), _button2->size());
	_button1->setFrame(floorv(b1pos), maxButtonSize);
	_button2->setFrame(floorv(b2pos), maxButtonSize);
	
	float topOrigin = etMin(_button1->origin().y, _button2->origin().y);
	float verticalCenter = 0.5f * (bgSize.y - topOrigin);
	
	_text->setPosition(floorv(vec2(0.5f * bgSize.x, verticalCenter)));
	_imgImage->setSize(contentWidth, contentHeight - buttonsSize.y);
	_imgImage->setPosition(0.5f * bgSize.x, verticalCenter);
}

void MessageView::setText(const std::string& text)
{
    _text->setText(text);
}

void MessageView::setBackgroundImage(const Image& img)
{
	_background->setImage(img);
}

void MessageView::setImage(const Image& img)
{
	_imgImage->setImage(img);
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