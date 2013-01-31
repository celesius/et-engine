/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/layout.h>
#include <et/gui/textfield.h>

using namespace et;
using namespace et::gui;

const short caretChar = '|';
const short securedChar = '*';

TextField::TextField(const Image& background, const std::string& text, Font font, Element* parent) : 
	Element2d(parent), _font(font), _background(background), _text(text), _secured(false),
	_caretVisible(false)
{
	setFlag(ElementFlag_RequiresKeyboard);
	setSize(font->measureStringSize(text));
	_caretBlinkTimer.expired.connect(this, &TextField::onCreateBlinkTimerExpired);
}

void TextField::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, gr);
	
	if (_backgroundVertices.offset() > 0)
		gr.addVertices(_backgroundVertices, _background.texture, ElementClass_2d, GuiRenderLayer_Layer0);

	if (_imageVertices.offset() > 0)
		gr.addVertices(_imageVertices, _background.texture, ElementClass_2d, GuiRenderLayer_Layer0);
	
	if (_textVertices.offset() > 0)
		gr.addVertices(_textVertices, _font->texture(), ElementClass_2d, GuiRenderLayer_Layer1);
}

void TextField::buildVertices(RenderContext*, GuiRenderer& gr)
{
	vec4 alphaVector = vec4(1.0f, 1.0f, 1.0f, alpha());
	mat4 transform = finalTransform();
	rect wholeRect(vec2(0.0), size());

	_backgroundVertices.setOffset(0);
	_imageVertices.setOffset(0);
	_textVertices.setOffset(0);
	
	if (_backgroundColor.w > 0.0f)
	{
		gr.createColorVertices(_backgroundVertices, wholeRect, _backgroundColor,
			transform, GuiRenderLayer_Layer0);
	}
	
	if (_background.texture.valid())
	{
		gr.createImageVertices(_imageVertices, _background.texture, _background.descriptor,
			wholeRect, alphaVector, transform, GuiRenderLayer_Layer0);
	}

	_charList = _secured ?
		CharDescriptorList(_text.length(), _font->charDescription(securedChar)) :
		_font->buildString(_text);
	
	vec2 textSize = _charList.size() ?
		_font->measureStringSize(_charList) : vec2(0.0f, _font->lineHeight());

	if (_caretVisible)
	{
		_charList.push_back(_font->charDescription(caretChar));
	}
	
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
		switch (msg.param.szValue)
		{
		case ET_RETURN:
		case ET_NEWLINE:
			{
				owner()->setActiveElement(nullptr);
				break;
			}
				
		case ET_BACKSPACE:
			{
				if (_text.length())
					_text = _text.substr(0, _text.length() - 1);
				textChanged.invoke(this);
				break;
			}

		default:
			{
				char text[2] = { static_cast<char>(msg.param.szValue & 0xff), 0 };
				_text += text;
				textChanged.invoke(this);
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
	
	editingStarted.invoke(this);
}

void TextField::resignFocus(Element*)
{
	_caretBlinkTimer.cancelUpdates();
	_caretVisible = false;
	invalidateContent();
	
	editingFinished.invoke(this);
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

void TextField::setBackgroundColor(const vec4& color)
{
	_backgroundColor = color;
	invalidateContent();
}
