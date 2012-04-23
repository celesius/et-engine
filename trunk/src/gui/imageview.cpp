#include <et/gui/imageview.h>

using namespace et;
using namespace gui;

ImageView::ImageView(const Texture& texture, Element2D* parent) : 	Element2D(parent), 
	_texture(texture), _descriptor(ImageDescriptor(texture)), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size);
}

ImageView::ImageView(const Texture& texture, const ImageDescriptor& i, Element2D* parent) : 
	Element2D(parent), _texture(texture), _descriptor(i), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size, 0.0f);
}

ImageView::ImageView(const Image& img, Element2D* parent) : 
	Element2D(parent), _texture(img.texture), _descriptor(img.descriptor), _contentMode(ImageView::ContentMode_Stretch)
{
	setSize(_descriptor.size, 0.0f);
}

void ImageView::addToRenderQueue(RenderContext* rc, GuiRenderer& g)
{
	if (!contentValid())
		buildVertices(rc, g);

	if (_vertices.size())
		g.addVertices(_vertices, _texture, ElementClass_2d, GuiRenderLayer_Layer0);
}

void ImageView::buildVertices(RenderContext*, GuiRenderer& g)
{
	_vertices.setOffset(0);
	
	if (_backgroundColor.w > 0.0f)
		g.createColorVertices(_vertices, rect(vec2(0.0f), size()), _backgroundColor, finalTransform(), GuiRenderLayer_Layer1);
	
	if (!_texture.valid()) return;

	mat4 transform = finalTransform();
	float imageWidth = _descriptor.size.x;
	float imageHeight = _descriptor.size.y;

	if (_contentMode == ContentMode_Center)
	{
		vec2 dp = 0.5f * (size() - _descriptor.size);
		g.createImageVertices(_vertices, _texture, _descriptor, rect(dp, _descriptor.size), 
			color(), transform, GuiRenderLayer_Layer0);
	}
	else if (_contentMode == ContentMode_Fill)
	{
		ImageDescriptor desc(_descriptor);
		rect frame(vec2(0.0f), size());

		bool fitToWidth = (frame.width > frame.height) && (desc.size.x > desc.size.y);

		if (fitToWidth)
		{
		}
		else
		{

		}

		g.createImageVertices(_vertices, _texture, desc, frame, color(), transform, GuiRenderLayer_Layer0);
	}
	else if (_contentMode == ContentMode_Tile)
	{
		int repeatsWidth = static_cast<int>(size().x / imageWidth);
		int repeatsHeight = static_cast<int>(size().y / imageHeight);
		int verticesPerImage = g.measuseVerticesCountForImageDescriptor(_descriptor);
		_vertices.resize(repeatsWidth * repeatsHeight * verticesPerImage);

		_vertices.setOffset(0);
		for (int v = 0; v < repeatsHeight; ++v)
		{
			for (int u = 0; u < repeatsWidth; ++u)
			{
				float fx = static_cast<float>(u * imageWidth);
				float fy = static_cast<float>(v * imageHeight);
				g.createImageVertices(_vertices, _texture, _descriptor, rect(vec2(fx, fy), _descriptor.size), 
					color(), transform, GuiRenderLayer_Layer0);
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

		 g.createImageVertices(_vertices, _texture, desc, rect(vec2(0.0f), size()), color(), transform, GuiRenderLayer_Layer0);
	}
	else
	{
		g.createImageVertices(_vertices, _texture, _descriptor, rect(vec2(0.0f), size()), color(), transform, GuiRenderLayer_Layer0);
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