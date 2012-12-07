/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/gui/element2d.h>
#include <et/gui/font.h>

namespace et
{
	namespace gui
	{
		class Label : public Element2d
		{
		public:
			typedef IntrusivePtr<Label> Pointer;

		public:
			Label(const std::string& text, Font font, Element2d* parent);

			vec2 textSize();
			
			Font font()
				{ return _font; }
			
			const std::string& text() const
				{ return _text; }
			
			const vec4& backgroundColor() const
				{ return _backgroundColor; }
			
			void addToRenderQueue(RenderContext* rc, GuiRenderer& guiRenderer);
			void fitToWidth(float);
			void adjustSize();
			
			void setAllowFormatting(bool);
			void setBackgroundColor(const vec4&);
			
			void setHorizontalAlignment(ElementAlignment);
			void setVerticalAlignment(ElementAlignment);
			
			void setShadowColor(const vec4& color);
			void setShadowOffset(const vec2& offset);
			void setText(const std::string& text, float duration = 0.0f);

		private:
			void buildVertices(RenderContext* rc, GuiRenderer& guiRenderer);
			void update(float t);

		private:
			std::string _text;
			std::string _nextText;
			
			CharDescriptorList _charListText;
			CharDescriptorList _charListNextText;
			
			Font _font;
			GuiVertexList _vertices;
			vec4 _backgroundColor;
			vec4 _shadowColor;
			vec2 _textSize;
			vec2 _nextTextSize;
			vec2 _shadowOffset;
			float _textFade;
			float _textFadeDuration;
			float _textFadeStartTime;
			ElementAlignment _horizontalAlignment;
			ElementAlignment _verticalAlignment;
			bool _animatingText;
			bool _allowFormatting;
		};
	}
}