#pragma once

#include <et/gui/element2d.h>
#include <et/gui/font.h>

namespace et
{
	namespace gui
	{
		class TextField : public Element2D
		{
		public:
			typedef IntrusivePtr<TextField> Pointer;

		public:
			TextField(const Image& background, const std::string& text, const Font& font, 
				Element* parent);

			void addToRenderQueue(RenderContext*, GuiRenderer&);

			const std::string& text() const;
			void setText(const std::string& s);

			void processMessage(const GuiMessage& msg);
			void setSecured(bool);

			void setFocus();
			void resignFocus(Element*);

		private:
			void buildVertices(RenderContext*, GuiRenderer&);
			void onCreateBlinkTimerExpired(NotifyTimer* t);

		private:
			Font _font;
			Image _background;
			std::string _text;
			CharDescriptorList _charList;
			GuiVertexList _imageVertices;
			GuiVertexList _textVertices;
			NotifyTimer _caretBlinkTimer;
			bool _secured;
			bool _caretVisible;
		};
	}
}