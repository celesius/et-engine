#pragma once

#include <et/gui/element2d.h>
#include <et/gui/guirenderer.h>

namespace et
{
	namespace gui
	{
		class ImageView : public Element2D
		{
		public:
			typedef IntrusivePtr<ImageView> Pointer;

		public:
			enum ContentMode
			{
				ContentMode_Stretch,
				ContentMode_Center,
				ContentMode_Fill,
				ContentMode_Tile,
				ContentMode_Crop
			};

		public:
			ImageView(const Texture& texture, Element2D* parent);
			ImageView(const Texture& texture, const ImageDescriptor& i, Element2D* parent);
			ImageView(const Image&, Element2D* parent);

			void addToRenderQueue(RenderContext*, GuiRenderer&);

			const Texture& texture() const 
				{ return _texture; }

			Texture& texture() 
				{ return _texture; }

			const ImageDescriptor& imageDescriptor() const
				{ return _descriptor; }

			const vec4& backgroundColor() const
				{ return _backgroundColor; }
			
			void setImageDescriptor(const ImageDescriptor& d);
			void setContentMode(ImageView::ContentMode cm);
			void setTexture(const Texture& t, bool updateDescriptor);
			void setImage(const Image& img);
			void setBackgroundColor(const vec4& color);

		private:
			void buildVertices(RenderContext*, GuiRenderer&);

		private:
			Texture _texture;
			GuiVertexList _vertices;
			ImageDescriptor _descriptor;
			ImageView::ContentMode _contentMode;
			vec4 _backgroundColor;
		};

		typedef std::vector<ImageView::Pointer> ImageViewList;
	}
}