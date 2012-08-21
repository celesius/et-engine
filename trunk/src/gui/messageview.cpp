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

MessageView::MessageView(const std::string& title, const std::string& text, Font font, const Image& bgImage,
						 const std::string& button1title, const std::string& button2title)
{
	_imgBackground = ImageView::Pointer(new ImageView(bgImage, backgroundFade().ptr()));
	
	_imgImage = ImageView::Pointer(new ImageView(Texture(), _imgBackground.ptr()));
	_text = Label::Pointer(new Label(text, font, _imgBackground.ptr()));
	_title = Label::Pointer(new Label("<b>" + title + "</b>", font, _imgBackground.ptr()));
	_button1 = Button::Pointer(new Button(button1title, font, _imgBackground.ptr()));
	_button2 = Button::Pointer(new Button(button2title, font, _imgBackground.ptr()));
	_buttonCommon = Button::Pointer(new Button(std::string(), font, backgroundFade().ptr()));
	
	_imgBackground->setPivotPoint(vec2(0.5f));
	
	_imgImage->setPivotPoint(vec2(0.5f, 0.0f));
	_imgImage->setContentMode(ImageView::ContentMode_Fit);
	
	_text->setPivotPoint(vec2(0.5f, 0.0f));
	_text->setHorizontalAlignment(ElementAlignment_Center);
	_text->setShadowColor(vec4(0.0f, 0.0f, 0.0f, 0.75f));

	_title->setPivotPoint(vec2(0.5f, 0.0f));
	_title->setHorizontalAlignment(ElementAlignment_Center);
	_title->setAllowFormatting(true);
	_title->setShadowColor(vec4(0.0f, 0.0f, 0.0f, 0.75f));
	
	_button2->clicked.connect(this, &MessageView::buttonClicked);
	_button1->clicked.connect(this, &MessageView::buttonClicked);
	_buttonCommon->clicked.connect(this, &MessageView::buttonClicked);
}

void MessageView::layout(const vec2& sz)
{
	_button1->setPivotPoint(hasSecondButton() ? vec2(0.0f, 1.0f) : vec2(0.5f, 1.0f));
	_button2->setPivotPoint(vec2(1.0f, 1.0f));
	_buttonCommon->setVisible(!(hasFirstButton() || hasSecondButton()));

	_button1->setVisible(hasFirstButton());
	_button2->setVisible(hasSecondButton());
	
	backgroundFade()->setFrame(vec2(0.0f), sz);
	_buttonCommon->setFrame(vec2(0.0f), sz);
	
	vec2 textSize = _text->textSize();
	if (textSize.x > 0.81f * sz.x)
		_text->fitToWidth(0.81f * sz.x);
	
	bool hasTitle = _title->textSize().dotSelf() > 0.0f;
	bool hasText = _text->textSize().dotSelf() > 0.0f;
	bool hasImage = _imgImage->texture().valid();
	bool hasButtons = hasFirstButton() || hasSecondButton();
	
	assert(!(hasText && hasImage) && "Unsupported MessageView parameters specified");
	
	float extraElements = static_cast<float>(hasTitle) + static_cast<float>(hasText || hasImage) + static_cast<float>(hasButtons);
	float gapsOffset = 2.0f + 0.5f * etMax(0.0f, extraElements - 1.0f);

	vec2 edgeOffset = _title->font()->measureStringSize("A");
	vec2 contentSize = _title->textSize() + vec2(0.0f, gapsOffset * edgeOffset.y);

	float gap = edgeOffset.y;
	float halfGap = 0.5f * edgeOffset.y;
	
	contentSize += vec2(_imgImage->imageDescriptor().size.x + _text->textSize().x,
						etMax(_imgImage->imageDescriptor().size.y, _text->textSize().y));
	
	if (hasFirstButton() || hasSecondButton())
	{
		contentSize.x = etMax(contentSize.x, (hasFirstButton() ? _button1->size().x : 0.0f) +
							  (hasSecondButton() ? _button2->size().x : 0.0f));
		contentSize.y += etMax(_button1->size().y, _button2->size().y);
	}
	
	contentSize.x += 2.0f * edgeOffset.x;
	
	contentSize.x = clamp(contentSize.x, _imgBackground->imageDescriptor().size.x, 0.9f * sz.x);
	contentSize.y = clamp(contentSize.y, _imgBackground->imageDescriptor().size.y, 0.9f * sz.y);

	_imgBackground->setPosition(floorv(0.5f * sz + _contentOffset));
	_imgBackground->setSize(contentSize);
	
	vec2 button1Pos(0.0f, contentSize.y - edgeOffset.y);
	vec2 button2Pos(0.0f, contentSize.y - edgeOffset.y);
	vec2 buttonsSize(0.0f, etMax(_button1->size().y, _button2->size().y));
	
	if (hasFirstButton())
	{
		if (hasSecondButton())
		{
			_button1->setPivotPoint(vec2(0.0f, 1.0f));
			_button2->setPivotPoint(vec2(1.0f, 1.0f));
			button1Pos.x = edgeOffset.x;
			button2Pos.x = contentSize.x - edgeOffset.x;
			buttonsSize.x = 0.5f * (contentSize.x - 3.0f * edgeOffset.x);
		}
		else
		{
			_button1->setPivotPoint(vec2(0.5f, 1.0f));
			button1Pos.x = 0.5f * contentSize.x;
			buttonsSize.x = contentSize.x - 2.0f * edgeOffset.x;
		}
	}
	else if (hasSecondButton())
	{
		_button2->setPivotPoint(vec2(0.5f, 1.0f));
		button2Pos.x = 0.5f * contentSize.x;
		buttonsSize.x = contentSize.x - 2.0f * edgeOffset.x;
	}
	
	_button1->setFrame(button1Pos - _contentOffset, buttonsSize);
	_button2->setFrame(button2Pos - _contentOffset, buttonsSize);
	
	float yPosition = gap;
	_title->setPosition(vec2(0.5f * contentSize.x, yPosition) - _contentOffset);
	
	yPosition += hasTitle ? halfGap + _title->textSize().y : 0.0f;
	if (hasText)
	{
		_text->setPosition(vec2(0.5f * contentSize.x, yPosition) - _contentOffset);
	}
	else if (hasImage)
	{
		vec2 imageSize(contentSize.x - 2.0f * edgeOffset.x, contentSize.y - yPosition - gap);
		
		if (hasButtons)
			imageSize.y -= gap + buttonsSize.y;
		
		_imgImage->setPosition(vec2(0.5f * contentSize.x, yPosition) - _contentOffset);
		_imgImage->setSize(imageSize);
	}
}

void MessageView::setText(const std::string& aText)
{
    _text->setText(aText);
}

void MessageView::setTitle(const std::string& aTitle)
{
    _title->setText("<b>" + aTitle + "</b>");
}

void MessageView::setBackgroundImage(const Image& img)
{
	_imgBackground->setImage(img);
}

void MessageView::setImage(const Image& img)
{
	_imgImage->setImage(img);
}

void MessageView::setButtonsBackground(const Image& img, ElementState s)
{
	_button1->setBackgroundForState(img, s);
	_button2->setBackgroundForState(img, s);
	
	_button1->adjustSize();
	_button2->adjustSize();
}

void MessageView::setButtonsTextColor(const vec4& color)
{
	_button1->setTextColor(color);
	_button2->setTextColor(color);
}

void MessageView::setButtonsPressedTextColor(const vec4& color)
{
	_button1->setTextPressedColor(color);
	_button2->setTextPressedColor(color);
}

bool MessageView::hasFirstButton() const
{
	return _button1->title().size() > 0;
}

bool MessageView::hasSecondButton() const
{
	return _button2->title().size() > 0;
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

void MessageView::setButton1Title(const std::string& t)
{
	_button1->setTitle(t);
}

void MessageView::setButton2Title(const std::string& t)
{
	_button2->setTitle(t);
}