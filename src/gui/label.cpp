/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/label.h>
#include <et/gui/guirenderer.h>

using namespace et;
using namespace et::gui;

Label::Label(const std::string& text, const Font& font, Element2D* parent) : 
	Element2D(parent), _text(text), _nextText(text), _font(font), _vertices(0),
	_backgroundColor(0.0f), _textFade(0.0f), _textFadeDuration(0.0f), _textFadeStartTime(0.0f),
	_horizontalAlignment(ElementAlignment_Near), _verticalAlignment(ElementAlignment_Near),
	_animatingText(false), _allowFormatting(false)
{
	adjustSize();
}

void Label::addToRenderQueue(RenderContext* rc, GuiRenderer& guiRenderer) 
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, guiRenderer);

	if (_vertices.currentIndex())
		guiRenderer.addVertices(_vertices, _font->texture(), ElementClass_2d, GuiRenderLayer_Layer1);
}

void Label::buildVertices(RenderContext*, GuiRenderer& renderer)
{
	mat4 transform = finalTransform();

	vec2 textOffset = size() * vec2(alignmentFactor(_horizontalAlignment), alignmentFactor(_verticalAlignment));
	
	_textSize = _font->measureStringSize(_text, _allowFormatting);
	_vertices.resize(0);
	
	if (_backgroundColor.w > 0.0f)
		renderer.createColorVertices(_vertices, rect(vec2(0.0f), size()), _backgroundColor, transform, GuiRenderLayer_Layer0);

	if (_animatingText)
	{
		float fadeIn = sqrt(_textFade);
		float fadeOut = 1.0f - sqr(_textFade);

		_nextTextSize = _font->measureStringSize(_nextText, _allowFormatting);

		renderer.createStringVertices(_vertices, _font->buildString(_text, _allowFormatting), _horizontalAlignment, _verticalAlignment, textOffset, 
									  color() * vec4(1.0, 1.0, 1.0, fadeOut), transform, GuiRenderLayer_Layer1);
		
		renderer.createStringVertices(_vertices, _font->buildString(_nextText, _allowFormatting), _horizontalAlignment, _verticalAlignment, textOffset, 
									  color() * vec4(1.0, 1.0, 1.0, fadeIn), transform, GuiRenderLayer_Layer1);
	}
	else 
	{
		_nextTextSize = _textSize;
		renderer.createStringVertices(_vertices, _font->buildString(_text, _allowFormatting), _horizontalAlignment, _verticalAlignment, 
									  textOffset, color(), transform, GuiRenderLayer_Layer1);
	}

	setContentValid();
}

void Label::setText(const std::string& text, float duration)
{
	if (text == _text) return;

	if (duration == 0.0f)
	{
		_text = text;
		adjustSize();
	}
	else 
	{
		startUpdates();

		if (_animatingText)
			_text = _nextText;

		_nextText = text;
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
		_nextText = std::string();
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
	invalidateContent();
}

void Label::setHorizontalAlignment(ElementAlignment h)
{
	if (_horizontalAlignment == h) return;
	
	_horizontalAlignment = h;
	invalidateContent();
}

void Label::setBackgroundColor(const vec4& color)
{
	_backgroundColor = color;
	invalidateContent();
}
