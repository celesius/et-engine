/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Additional thanks to Kirill Polezhaiev
 * Please, do not modify content without approval.
 *
 */

#include <et/gui/guirenderer.h>
#include <et/gui/slider.h>

using namespace et;
using namespace et::gui;

Slider::Slider(Element2d* parent) : Element2d(parent)
{   
    _min = 0.0f;
    _max = 1.0f;
    _value = 0.5f;
    _fillingLineWidth = 16;
    _selected = false;
}

void Slider::setValue(float value)
{
	_value = (et::clamp(value, _min, _max) - _min) / (_max - _min);
}

float Slider::value() const
{
	return _min + ((_max - _min) * _value);
}

void Slider::addToRenderQueue(RenderContext* renderContext, GuiRenderer& guiRenderer)
{    
	if (!contentValid() || !transformValid())
		buildVertices(renderContext, guiRenderer);
    
    guiRenderer.addVertices(_backgroundVertices, _backgroundImage.texture,
		ElementClass_2d, GuiRenderLayer_Layer0);
	
    guiRenderer.addVertices(_fillingLineVertices, _fillingLineImage.texture,
		ElementClass_2d, GuiRenderLayer_Layer0);
	
    guiRenderer.addVertices(_handleVertices, _handleImage.texture,
		ElementClass_2d, GuiRenderLayer_Layer0);
}

void Slider::setImages(Image background, Image fillingLine, Image handle)
{
    _backgroundImage = background;
    _fillingLineImage = fillingLine;
    _handleImage = handle;
}

void Slider::buildVertices(RenderContext* renderContext, GuiRenderer& guiRenderer)
{
	vec4 white = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 gray = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	vec4 backgroundColor = vec4(0.5f, 1.0f, 0.5f, 1.0f);
	vec4 fillingLineColor = vec4(0.5f, 0.75f, 1.0f, 1.0f);
	vec4 handleColor = _selected ? gray : white;
    
    rect backgroundRect = rect(0.0f, (size().y - _fillingLineWidth) * 0.5f,
		size().x, _fillingLineWidth);
	
    rect fillingLineRect = rect(0.0f, (size().y - _fillingLineWidth) * 0.5f,
		(size().x - size().y) * _value + size().y * 0.5f, _fillingLineWidth);
	
    rect handleRect = rect((size().x - 0.5f * size().y) * _value, 0, size().y, size().y);
    
    _backgroundVertices.setOffset(0);
    _fillingLineVertices.setOffset(0);
    _handleVertices.setOffset(0);
    
    mat4 transform = finalTransform();
    
    if (_backgroundImage.texture.valid())
	{
        guiRenderer.createImageVertices(_backgroundVertices, _backgroundImage.texture,
			_backgroundImage.descriptor, backgroundRect, white, transform, GuiRenderLayer_Layer0);
	}
    else
	{
        guiRenderer.createColorVertices(_backgroundVertices, backgroundRect, backgroundColor,
			transform, GuiRenderLayer_Layer0);
	}
	
    if (_fillingLineImage.texture.valid())
	{
        guiRenderer.createImageVertices(_fillingLineVertices, _fillingLineImage.texture,
			_fillingLineImage.descriptor, fillingLineRect, white, transform, GuiRenderLayer_Layer0);
	}
    else
	{
        guiRenderer.createColorVertices(_fillingLineVertices, fillingLineRect, fillingLineColor,
			transform, GuiRenderLayer_Layer0);
	}
	
    if (_fillingLineImage.texture.valid())
	{
        guiRenderer.createImageVertices(_handleVertices, _handleImage.texture,
			_handleImage.descriptor, handleRect, handleColor, transform, GuiRenderLayer_Layer0);
	}
    else
	{
        guiRenderer.createColorVertices(_handleVertices, handleRect, handleColor,
			transform, GuiRenderLayer_Layer0);
	}
    
	setContentValid();
}

bool Slider::pointerPressed(const PointerInputInfo& info)
{
    _selected = true;
    _value = et::clamp((info.pos.x - 0.5f * size().y) / size().x, 0.0f, 1.0f);
    invalidateContent();
    valueChanged.invoke(this);
    return true;
}

bool Slider::pointerMoved(const PointerInputInfo& info)
{
    if (_selected)
    {
        _value = et::clamp((info.pos.x - 0.5f * size().y) / size().x, 0.0f, 1.0f);
        invalidateContent();
        valueChanged.invoke(this);
    }
	
    return true;
}

bool Slider::pointerReleased(const PointerInputInfo&)
{
    _selected = false;
    invalidateContent();
    return true;
}

bool Slider::pointerCancelled(const PointerInputInfo&)
{
    _selected = false;
    invalidateContent();
    return true;   
}
