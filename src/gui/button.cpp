/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/button.h>
#include <et/gui/guirenderer.h>

using namespace et;
using namespace et::gui;

Button::Button(const std::string& title, Font font, Element2d* parent) : Element2d(parent), 
	_title(title), _font(font), _textSize(font->measureStringSize(title, true)), _textColor(vec3(0.0f), 1.0f),
	_textPressedColor(vec3(0.0f), 1.0f), _type(Button::Type_PushButton), _state(ElementState_Default), 
	_imageLayout(ImageLayout_Left),	_pressed(false), _hovered(false), _selected(false)
{
	setSize(sizeForText(title));
}

void Button::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, gr);

	if (_bgVertices.offset() > 0)
		gr.addVertices(_bgVertices, _background[_state].texture, ElementClass_2d, GuiRenderLayer_Layer0);

	if (_textVertices.offset() > 0)
		gr.addVertices(_textVertices, _font->texture(), ElementClass_2d, GuiRenderLayer_Layer1);

	if (_imageVertices.offset() > 0)
		gr.addVertices(_imageVertices, _image.texture, ElementClass_2d, GuiRenderLayer_Layer0);
}

void Button::buildVertices(RenderContext*, GuiRenderer& gr)
{
	mat4 transform = finalTransform();

	vec2 imageSize = _image.descriptor.size;
	vec2 actualSize = size() + _contentOffset;
	float maxImageDimension = etMax(imageSize.x, imageSize.y);
	float maxDimension = etMax(actualSize.x, actualSize.y);
	float aspect = maxImageDimension / maxDimension;
	
	if (aspect > 1.0f)
		imageSize /= aspect;
	
	float contentGap = (imageSize.x > 0.0f) && (_textSize.x > 0.0f) ? 5.0f : 0.0f;
	float contentWidth = imageSize.x + _textSize.x + contentGap;

	vec2 imageOrigin;
	vec2 textOrigin;
	
	if (_imageLayout == ImageLayout_Right)
	{
		textOrigin = 0.5f * (actualSize - vec2(contentWidth, _textSize.y));
		imageOrigin = vec2(textOrigin.x + contentGap + _textSize.x, 0.5f * (actualSize.y - imageSize.y));
	}
	else
	{
		imageOrigin = 0.5f * (actualSize - vec2(contentWidth, imageSize.y));
		textOrigin = vec2(imageOrigin.x + contentGap + imageSize.x, 0.5f * (actualSize.y - _textSize.y));
	}
	vec4 alphaScaleColor = vec4(1.0f, 1.0f, 1.0f, alpha());
	
	_bgVertices.setOffset(0);
	_textVertices.setOffset(0);
	_imageVertices.setOffset(0);
	
	if (_backgroundColor.w > 0.0f)
	{
		gr.createColorVertices(_bgVertices, rect(vec2(0.0f), size()), _backgroundColor, transform, GuiRenderLayer_Layer0);
	}

	if (_background[_state].texture.valid())
	{
		gr.createImageVertices(_bgVertices, _background[_state].texture, _background[_state].descriptor,
			rect(vec2(0.0f), size()), color(), transform, GuiRenderLayer_Layer0);
	}

	if (_title.size() > 0)
	{
		vec4 aColor = _state == ElementState_Pressed ? _textPressedColor : _textColor;
		if (aColor.w > 0.0f)
		{
			gr.createStringVertices(_textVertices, _font->buildString(_title, true), ElementAlignment_Near, ElementAlignment_Near,
				textOrigin, aColor * alphaScaleColor, transform, GuiRenderLayer_Layer1);
		}
	}

	if (_image.texture.valid())
	{
		vec4 aColor = _state == ElementState_Pressed ?
			color() * vec4(0.5f, 0.5f, 0.5f, 1.0f) : color();
		
		if (aColor.w > 0.0f)
		{
			gr.createImageVertices(_imageVertices, _image.texture, _image.descriptor, 
				rect(imageOrigin, imageSize), aColor * alphaScaleColor, transform, GuiRenderLayer_Layer0);
		}
	}
}

void Button::setBackgroundForState(const Texture& tex, const ImageDescriptor& desc, ElementState s)
{
	setBackgroundForState(Image(tex, desc), s);
}

void Button::setBackgroundForState(const Image& img, ElementState s)
{
	_background[s] = img;
	invalidateContent();
}

bool Button::pointerPressed(const PointerInputInfo& p)
{
	if (p.type != PointerType_General) return false;

	_pressed = true;
	setCurrentState(_selected ? ElementState_SelectedPressed : ElementState_Pressed);
	
	pressed.invoke(this);
	return true;
}

bool Button::pointerReleased(const PointerInputInfo& p)
{
	if ((p.type != PointerType_General) || !_pressed) return false;
	
	_pressed = false;
	ElementState newState = _selected ? ElementState_Selected : ElementState_Default;

	if (containLocalPoint(p.pos))
	{
		performClick();
		newState = adjustElementState(_selected ? ElementState_SelectedHovered : ElementState_Hovered);
	}

	setCurrentState(newState);
	released.invoke(this);
	return true;
}

bool Button::pointerCancelled(const PointerInputInfo& p)
{
	if ((p.type != PointerType_General) || !_pressed) return false;
	
	_pressed = false;
	ElementState newState = newState = _selected ? ElementState_Selected : ElementState_Default;
	
	if (containLocalPoint(p.pos))
		newState = adjustElementState(_selected ? ElementState_SelectedHovered : ElementState_Hovered);
	
	setCurrentState(newState);
	released.invoke(this);
	return true;
}

void Button::pointerEntered(const PointerInputInfo&)
{
	if (_selected)
		setCurrentState(adjustElementState(_pressed ? ElementState_SelectedPressed : ElementState_SelectedHovered));
	else
		setCurrentState(adjustElementState(_pressed ? ElementState_Pressed : ElementState_Hovered));
}

void Button::pointerLeaved(const PointerInputInfo&)
{
	if (_selected)
		setCurrentState(_pressed ? ElementState_SelectedPressed : ElementState_Selected);
	else
		setCurrentState(_pressed ? ElementState_Pressed : ElementState_Default);
}

void Button::setCurrentState(ElementState s)
{
	if (_state == s) return;

	_state = s;
	invalidateContent();
}

bool Button::capturePointer() const
{
	return true;
}

void Button::performClick()
{
	if (_type == Type_CheckButton)
		setSelected(!_selected);
	
	clicked.invoke(this);
}

const vec2& Button::textSize()
{
	return _textSize; 
}

void Button::setTitle(const std::string& t)
{
	if (_title == t) return;

	_title = t;
	_textSize = _font->measureStringSize(_title, true);
	invalidateContent();
}

void Button::setTextColor(const vec4& color)
{
	_textColor = color;
	invalidateContent();
}

const vec4& Button::textColor() const
{
	return _textColor;
}

void Button::setTextPressedColor(const vec4& color)
{
	_textPressedColor = color;
	invalidateContent();
}

const vec4& Button::textPressedColor() const
{
	return _textPressedColor;
}

void Button::adjustSize(float duration)
{
	adjustSizeForText(_title, duration);
}

void Button::adjustSizeForText(const std::string& text, float duration)
{
	setSize(sizeForText(text), duration);
}

vec2 Button::sizeForText(const std::string& text)
{
	vec2 textSize = _font->measureStringSize("AA" + text + "AA", true);
	
	for (size_t i = 0; i < ElementState_max; ++i)
		textSize = maxv(textSize, _background[i].descriptor.size);

	return vec2(floorf(textSize.x), floorf(1.25f * textSize.y));
}

void Button::setImage(const Image& img)
{
	_image = img;
	invalidateContent();
}

void Button::setSelected(bool s)
{
	bool wasSelected = _selected;
	_selected = s && (_type == Type_CheckButton);

	if (wasSelected != _selected)
	{
		if (elementIsSelected(_state))
			setCurrentState(static_cast<ElementState>(_state - 3 * static_cast<int>(!_selected)));
		else
			setCurrentState(static_cast<ElementState>(_state + 3 * static_cast<int>(_selected)));
	}
}

void Button::setType(Button::Type t)
{
	_type = t;
	setSelected(false);
}

void Button::setContentOffset(const vec2& o)
{
	_contentOffset = o;
	invalidateContent();
}

void Button::setImageLayout(ImageLayout l)
{
	_imageLayout = l;
	invalidateContent();
}

void Button::setBackgroundColor(const vec4& color)
{
	_backgroundColor = color;
	invalidateContent();
}

const vec4& Button::backgroundColor() const
{
	return _backgroundColor;
}
