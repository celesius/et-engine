/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/gui/imageview.h>

using namespace et;
using namespace gui;

ImageView::ImageView(const Texture& texture, Element2d* parent) : Element2d(parent),
	_texture(texture), _descriptor(ImageDescriptor(texture)), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size);
}

ImageView::ImageView(const Texture& texture, const ImageDescriptor& i, Element2d* parent) : 
	Element2d(parent), _texture(texture), _descriptor(i), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size, 0.0f);
}

ImageView::ImageView(const Image& img, Element2d* parent) : 
	Element2d(parent), _texture(img.texture), _descriptor(img.descriptor), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size, 0.0f);
}

void ImageView::addToRenderQueue(RenderContext* rc, GuiRenderer& g)
{
	if (!contentValid() || !transformValid())
		buildVertices(rc, g);

	if (_vertices.offset())
		g.addVertices(_vertices, _texture, ElementClass_2d, RenderLayer_Layer0);
}

void ImageView::buildVertices(RenderContext*, GuiRenderer& g)
{
	mat4 transform = finalTransform();
	_vertices.setOffset(0);
	
	if (_backgroundColor.w > 0.0f)
		g.createColorVertices(_vertices, rect(vec2(0.0f), size()), _backgroundColor, transform, RenderLayer_Layer1);
	
	if (!_texture.valid()) return;

	float imageWidth = _descriptor.size.x;
	float imageHeight = _descriptor.size.y;

	if (_contentMode == ContentMode_Center)
	{
		vec2 dp = 0.5f * (size() - _descriptor.size);
		g.createImageVertices(_vertices, _texture, _descriptor, rect(dp, _descriptor.size), 
			color(), transform, RenderLayer_Layer0);
	}
	else if (_contentMode == ContentMode_Fit)
	{
		vec2 frameSize = size();
		vec2 descSize = absv(_descriptor.size);
		
		if ((descSize.x > frameSize.x) || (descSize.y > frameSize.y))
		{
			float imageAspect = descSize.aspect();
			float frameAspect = frameSize.aspect();
			if (frameAspect > 1.0f)
			{
				if (imageAspect > 1.0f)
				{
					float resultHeight = frameSize.x / imageAspect;
					if (resultHeight > frameSize.y)
					{
						float scale = frameSize.y / resultHeight;
						frameSize.x *= scale;
						frameSize.y = resultHeight * scale;
					}
					else
					{
						frameSize.y = resultHeight;
					}
				}
				else
				{
					frameSize.x = frameSize.x * imageAspect / frameAspect;
				}
			}
			else
			{
				frameSize.y = frameSize.y / imageAspect * frameAspect;
			}
		}
		else
		{
			frameSize = descSize;
		}
		
		vec2 origin = 0.5f * (size() - frameSize);
		g.createImageVertices(_vertices, _texture, _descriptor, rect(origin, frameSize),
			color(), transform, RenderLayer_Layer0);
	}
	else if (_contentMode == ContentMode_Fill)
	{
		log::warning("ImageView::ContentMode_Fill is not supported yet.");
	}
	else if (_contentMode == ContentMode_Tile)
	{
		int repeatsWidth = static_cast<int>(size().x / imageWidth);
		int repeatsHeight = static_cast<int>(size().y / imageHeight);
		
		_vertices.fitToSize(repeatsWidth * repeatsHeight * g.measusevertexCountForImageDescriptor(_descriptor));
		_vertices.setOffset(0);
		
		for (int v = 0; v < repeatsHeight; ++v)
		{
			for (int u = 0; u < repeatsWidth; ++u)
			{
				float fx = static_cast<float>(u * imageWidth);
				float fy = static_cast<float>(v * imageHeight);
				g.createImageVertices(_vertices, _texture, _descriptor, rect(vec2(fx, fy), _descriptor.size), 
					color(), transform, RenderLayer_Layer0);
			}
		}
	}
	else if (_contentMode == ContentMode_Crop)
	{
		ImageDescriptor desc = _descriptor;
		vec2 dSize = desc.size;
		desc.size.x = size().x < desc.size.x ? size().x : dSize.x;
		desc.size.y = size().y < desc.size.y ? size().y : dSize.y;
		vec2 cropped = dSize - desc.size;
		desc.origin += cropped * pivotPoint();
		g.createImageVertices(_vertices, _texture, desc, rect(vec2(0.0f), size()), color(), transform, RenderLayer_Layer0);
	}
	else
	{
		g.createImageVertices(_vertices, _texture, _descriptor, rect(vec2(0.0f), size()), color(), transform, RenderLayer_Layer0);
	}

	setContentValid();
}

void ImageView::setImageDescriptor(const ImageDescriptor& d)
{
	_descriptor = d;
	invalidateContent();
}

void ImageView::setContentMode(ImageView::ContentMode cm)
{
	if (cm != _contentMode)
	{
		_contentMode = cm;
		invalidateContent();
	}
}

void ImageView::setTexture(const Texture& t, bool updateDescriptor)
{
	_texture = t;
	if (updateDescriptor)
		_descriptor = ImageDescriptor(t);
	invalidateContent();
}

void ImageView::setImage(const Image& img)
{
	_texture = img.texture;
	_descriptor = img.descriptor;
	invalidateContent();
}

void ImageView::setBackgroundColor(const vec4& color)
{
	_backgroundColor = color;
	invalidateContent();
}