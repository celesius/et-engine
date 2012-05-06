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
		class Label : public Element2D
		{
		public:
			typedef IntrusivePtr<Label> Pointer;

		public:
			Label(const std::string& text, const Font& font, Element2D* parent);

			void addToRenderQueue(RenderContext* rc, GuiRenderer& guiRenderer);
			void setText(const std::string& text, float duration = 0.0f);

			vec2 textSize();
			inline const std::string& text() const
				{ return _text; }

			void adjustSize();
			void setAllowFormatting(bool f);

		private:
			void buildVertices(RenderContext* rc, GuiRenderer& guiRenderer);
			void update(float t);

		private:
			std::string _text;
			std::string _nextText;
			Font _font;
			GuiVertexList _vertices;
			vec2 _textSize;
			vec2 _nextTextSize;
			float _textFade;
			float _textFadeDuration;
			float _textFadeStartTime;
			bool _animatingText;
			bool _allowFormatting;
		};

		typedef std::vector<Label::Pointer> LabelList;
	}
}