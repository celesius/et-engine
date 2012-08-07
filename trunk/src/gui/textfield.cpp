/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/textfield.h>
#include <et/gui/guirenderer.h>

using namespace et;
using namespace et::gui;

const short caretChar = '|';
const short securedChar = '*';

TextField::TextField(const Image& background, const std::string& text, Font font, Element* parent) : 
	Element2d(parent), _font(font), _background(background), _text(text), _secured(false), _caretVisible(false)
{
	setFlag(ElementFlag_RequiresKeyboard);
	_caretBlinkTimer.expired.connect(this, &TextField::onCreateBlinkTimerExpired);
}

void TextField::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, gr);

	if (_imageVertices.currentIndex())
		gr.addVertices(_imageVertices, _background.texture, ElementClass_2d, GuiRenderLayer_Layer0);
	
	if (_textVertices.currentIndex())
		gr.addVertices(_textVertices, _font->texture(), ElementClass_2d, GuiRenderLayer_Layer1);
}

void TextField::buildVertices(RenderContext*, GuiRenderer& gr)
{
	vec4 alphaVector = vec4(1.0f, 1.0f, 1.0f, alpha());
	mat4 transform = finalTransform();

	_imageVertices.setOffset(0);
	gr.createImageVertices(_imageVertices, _background.texture, _background.descriptor, 
		rect(vec2(0.0), size()), alphaVector, transform, GuiRenderLayer_Layer0);

	_charList = _secured ? CharDescriptorList(_text.length(), _font->charDescription(securedChar)) : 
							_font->buildString(_text);

	vec2 textSize = _charList.size() ? _font->measureStringSize(_charList) : vec2(0.0f, _font->lineHeight());

	if (_caretVisible)
		_charList.push_back(_font->charDescription(caretChar));

	_textVertices.setOffset(0);
	if (_charList.size())
	{
		gr.createStringVertices(_textVertices, _charList, ElementAlignment_Near, ElementAlignment_Near,
								0.5f * (size() - textSize), color() * alphaVector, transform, GuiRenderLayer_Layer1);
	}
	setContentValid();
}

void TextField::setText(const std::string& s)
{
	_text = s;
	invalidateContent();
}

void TextField::processMessage(const GuiMessage& msg)
{
	if (msg.type == GuiMessage::Type_TextInput)
	{
		switch (msg.p1.ucharValues[0])
		{
		case 8:
			{
				if (_text.length())
					_text = _text.substr(0, _text.length() - 1);
				break;
			}

		default:
			{
				char text[2] = { static_cast<char>(msg.p1.ucharValues[0]), 0 };
				_text += text;
			}
		}

		invalidateContent();
	}
}

void TextField::setSecured(bool s)
{
	_secured = s;
	invalidateContent();
}

void TextField::setFocus()
{
	_caretBlinkTimer.start(timerPool(), 0.5f, NotifyTimer::RepeatForever);
	_caretVisible = true;
	invalidateContent();
}

void TextField::resignFocus(Element*)
{
	_caretBlinkTimer.cancelUpdates();
	_caretVisible = false;
	invalidateContent();
}

void TextField::onCreateBlinkTimerExpired(NotifyTimer*)
{
	_caretVisible = !_caretVisible;
	invalidateContent();
}

const std::string& TextField::text() const
{
	return _text;
}