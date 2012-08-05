/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once 

#include <et/gui/Element2d.h>
#include <et/gui/font.h>

namespace et
{
	namespace gui
	{
		class Button : public Element2d
		{
		public:
			typedef IntrusivePtr<Button> Pointer;

			enum Type
			{
				Type_PushButton,
				Type_CheckButton
			};

			enum ImageLayout
			{
				ImageLayout_Left,
				ImageLayout_Right
			};

		public:
			Button(const std::string& title, Font font, Element2d* parent);

			void setBackgroundForState(const Texture& tex, const ImageDescriptor& desc, ElementState s);
			void setBackgroundForState(const Image& img, ElementState s);
			void addToRenderQueue(RenderContext* rc, GuiRenderer& guiRenderer);

			void adjustSize(float duration = 0.0f);
			void adjustSizeForText(const std::string&, float duration = 0.0f);
			vec2 sizeForText(const std::string&);

			ET_DECLARE_EVENT1(clicked, Button*)

			bool capturePointer() const;

			const std::string& title() const 
				{ return _title; }

			void setTitle(const std::string& t);

			const Image& image() const
				{ return _image; }
			void setImage(const Image& img);

			void setImageLayout(ImageLayout l);

			const vec2& textSize();

			void setTextColor(const vec4& color);
			const vec4& textColor() const;
			
			void setTextPressedColor(const vec4& color);
			const vec4& textPressedColor() const;

			bool pointerPressed(const PointerInputInfo&);
			bool pointerReleased(const PointerInputInfo&);
			bool pointerCancelled(const PointerInputInfo&);
			void pointerEntered(const PointerInputInfo&);
			void pointerLeaved(const PointerInputInfo&);

			Button::Type type() const
				{ return _type; }

			void setType(Button::Type t);

			bool selected() const
				{ return _selected; }
			void setSelected(bool s);

			void setContentOffset(const vec2& o);

		protected:
			void performClick();

		private:
			void buildVertices(RenderContext* rc, GuiRenderer& gr);
			void setCurrentState(ElementState s);

		private:
			Font _font;
			std::string _title;
			GuiVertexList _bgVertices;
			GuiVertexList _textVertices;
			GuiVertexList _imageVertices;
			StaticDataStorage<Image, ElementState_max> _background;
			Image _image;
			vec2 _textSize;
			vec2 _imageSize;
			vec4 _textColor;
			vec4 _textPressedColor;
			vec2 _contentOffset;

			Type _type;
			ElementState _state;
			ImageLayout _imageLayout;
			bool _pressed;
			bool _hovered;
			bool _selected;
		};
	}
}