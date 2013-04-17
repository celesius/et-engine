/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/tools.h>
#include <et/gui/guirenderer.h>
#include <et/gui/label.h>

using namespace et;
using namespace et::gui;

Label::Label(const std::string& text, Font font, Element2d* parent) :
	Element2d(parent), _text(text), _nextText(text), _font(font), _vertices(0),
	_backgroundColor(0.0f), _shadowOffset(1.0f), _textFade(0.0f), _textFadeDuration(0.0f), _textFadeStartTime(0.0f),
	_horizontalAlignment(ElementAlignment_Near), _verticalAlignment(ElementAlignment_Near),
	_animatingText(false), _allowFormatting(false)
{
	setFlag(ElementFlag_TransparentForPointer);
	_charListText = _font->buildString(_text);
	_charListNextText = _font->buildString(_nextText);
	adjustSize();
}

void Label::addToRenderQueue(RenderContext* rc, GuiRenderer& guiRenderer) 
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, guiRenderer);

	if (_vertices.offset() > 0)
		guiRenderer.addVertices(_vertices, _font->texture(), ElementClass_2d, RenderLayer_Layer1);
}

void Label::buildVertices(RenderContext*, GuiRenderer& renderer)
{
	mat4 transform = finalTransform();

	vec2 alignment = vec2(alignmentFactor(_horizontalAlignment),
						  alignmentFactor(_verticalAlignment));
	
	vec2 textOffset = size() * alignment;
	
	_vertices.setOffset(0);

	bool hasShadow = _shadowColor.w > 0.0f;
	
	if (_backgroundColor.w > 0.0f)
		renderer.createColorVertices(_vertices, rect(vec2(0.0f), size()), _backgroundColor, transform, RenderLayer_Layer0);

	if (_animatingText)
	{
		float fadeIn = sqrtf(_textFade);
		float fadeOut = 1.0f - sqr(_textFade);

		if (hasShadow)
		{
			vec2 shadowOffset = textOffset + _shadowOffset;
			vec4 shadowColor = _shadowColor;

			shadowColor.w *= color().w * fadeOut;
			renderer.createStringVertices(_vertices, _charListText, _horizontalAlignment, _verticalAlignment,
				shadowOffset, shadowColor, transform, RenderLayer_Layer1);
			
			shadowColor.w = _shadowColor.w * color().w * fadeIn;
			renderer.createStringVertices(_vertices, _charListNextText, _horizontalAlignment, _verticalAlignment,
				shadowOffset, shadowColor, transform, RenderLayer_Layer1);
		}

		renderer.createStringVertices(_vertices, _charListText, _horizontalAlignment, _verticalAlignment, textOffset, 
			color() * vec4(1.0, 1.0, 1.0, fadeOut), transform, RenderLayer_Layer1);
		
		renderer.createStringVertices(_vertices, _charListNextText, _horizontalAlignment, _verticalAlignment, textOffset,
			color() * vec4(1.0, 1.0, 1.0, fadeIn), transform, RenderLayer_Layer1);
	}
	else 
	{
		_nextTextSize = _textSize;

		if (hasShadow)
		{
			renderer.createStringVertices(_vertices, _charListText, _horizontalAlignment, _verticalAlignment, 
				textOffset + _shadowOffset, _shadowColor * vec4(1.0f, 1.0f, 1.0, color().w), transform, RenderLayer_Layer1);
		}

		renderer.createStringVertices(_vertices, _charListText, _horizontalAlignment, _verticalAlignment, 
			textOffset, color(), transform, RenderLayer_Layer1);
	}

	setContentValid();
}

void Label::setText(const std::string& text, float duration)
{
	if (text == _text) return;

	if (duration == 0.0f)
	{
		_text = text;
		_charListText = _font->buildString(_text, _allowFormatting);
		adjustSize();
	}
	else 
	{
		startUpdates();

		if (_animatingText)
			setText(_nextText, 0.0f);
		
		_nextText = text;
		_charListNextText = _font->buildString(_nextText, _allowFormatting);
		_nextTextSize = _font->measureStringSize(_nextText, _allowFormatting);
		
		_textFade = 0.0f;
		_animatingText = true;
		_textFadeStartTime = actualTime();
		_textFadeDuration = duration;
	}

	invalidateContent();
}

vec2 Label::textSize()
{
	if (!contentValid())
	{
		_textSize = _font->measureStringSize(_text, _allowFormatting);

		if (_animatingText)
			_nextTextSize = _font->measureStringSize(_nextText, _allowFormatting);
	}

	return _animatingText ? _nextTextSize : _textSize;
}

void Label::update(float t)
{
	_textFade = (t - _textFadeStartTime) / _textFadeDuration;
	if (_textFade >= 1.0f)
	{
		_textFade = 0.0f;
		_animatingText = false;
		
		_text = _nextText;
		_charListText = _font->buildString(_text, _allowFormatting);
		_textSize = _font->measureStringSize(_text, _allowFormatting);
		
		_nextText = std::string();
		_charListNextText.clear();
		_nextTextSize = vec2(0.0f);
		
		adjustSize();
		cancelUpdates();
	}

	invalidateContent();
}

void Label::adjustSize()
{
	_textSize = _font->measureStringSize(_text, _allowFormatting);
	setSize(_textSize);
}

void Label::setAllowFormatting(bool f)
{
	_allowFormatting = f;
	_charListNextText = _font->buildString(_nextText, _allowFormatting);
	_charListText = _font->buildString(_text, _allowFormatting);
	invalidateContent();
}

void Label::setHorizontalAlignment(ElementAlignment h)
{
	if (_horizontalAlignment == h) return;
	
	_horizontalAlignment = h;
	invalidateContent();
}

void Label::setVerticalAlignment(ElementAlignment v)
{
	if (_verticalAlignment == v) return;
	
	_verticalAlignment = v;
	invalidateContent();
}

void Label::setBackgroundColor(const vec4& color)
{
	_backgroundColor = color;
	invalidateContent();
}

void Label::setShadowColor(const vec4& color)
{
	_shadowColor = color;
	invalidateContent();
}

void Label::setShadowOffset(const vec2& offset)
{
	_shadowOffset = offset;
	invalidateContent();
}

void Label::fitToWidth(float w)
{
	std::string newText;
	std::string oldText = _text;
	std::string latestLine;
	
	const std::string dividers(" \n\t\r");
	
	while (oldText.size())
	{
		size_t wsPos = oldText.find_first_of(dividers);
		
		if (wsPos == std::string::npos)
		{
			std::string appended = latestLine + oldText;
			vec2 measuredSize = _font->measureStringSize(appended);
			newText.append((measuredSize.x < w) ? oldText : "\n" + oldText);
			break;
		}
		
		std::string word = oldText.substr(0, wsPos);
		
		char nextCharStr[] = { oldText[wsPos++], 0 };
		oldText.erase(0, wsPos);
		
		std::string appended = latestLine + word;
		if (!isNewLineChar(nextCharStr[0]))
			appended.append(nextCharStr);
		
		if (_font->measureStringSize(appended).x < w)
		{
			newText.append(word);
			latestLine.append(word);
			if (isNewLineChar(nextCharStr[0]))
			{
				latestLine = std::string();
				newText.append("\n");
			}
			else
			{
				newText.append(nextCharStr);
				latestLine.append(nextCharStr);
			}
		}
		else if (_font->measureStringSize(word).x > w)
		{
			std::string wBegin = word.substr(0, word.size() - 1);
			appended = latestLine + wBegin;
			while (_font->measureStringSize(appended).x > w)
			{
				wBegin.erase(wBegin.size() - 1);
				appended = latestLine + wBegin;
			}
			newText.append(appended);
			newText.append("\n");
			newText.append(word.substr(wBegin.size()));
		}
		else
		{
			size_t lastCharPos = newText.size() - 1;
			char lastChar = newText.at(lastCharPos);
			
			if (isWhitespaceChar(lastChar) && !isNewLineChar(lastChar))
			{
				newText[lastCharPos] = '\n';
				newText.append(word);
			}
			else
			{
				newText.append("\n" + word);
			}
			
			if (isNewLineChar(nextCharStr[0]))
			{
				latestLine = std::string();
				newText.append("\n");
			}
			else
			{
				newText.append(nextCharStr);
				latestLine = word + nextCharStr;
			}
		}
	}
	
	setText(newText);
}
