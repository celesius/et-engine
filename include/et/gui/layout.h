#pragma once

#include <et/gui/element2d.h>
#include <et/gui/element3d.h>
#include <et/gui/keyboard.h>

namespace et
{
	namespace gui
	{
		class Layout : public Element2D, public KeyboardDelegate
		{
		public:
			typedef IntrusivePtr<Layout> Pointer;

		public:
			Layout();

			bool valid() const
				{ return _valid; }

			void layout(const vec2& sz);

			void addToRenderQueue(RenderContext* rc, GuiRenderer& gr);

			void update(float);
			void cancelDragging(float returnDuration = 0.0f);

			virtual void adjustVerticalOffset(float dy);
			virtual void resetVerticalOffset();
			
			void setActiveElement(Element* e);

			ET_DECLARE_EVENT2(layoutRequiresKeyboard, Layout*, Element*)
			ET_DECLARE_EVENT1(layoutDoesntNeedKeyboard, Layout*)

		protected:
			friend class Gui;

			bool pointerPressed(const et::PointerInputInfo&);
			bool pointerMoved(const et::PointerInputInfo&);
			bool pointerReleased(const et::PointerInputInfo&);
			bool pointerScrolled(const et::PointerInputInfo&);

			void setInvalid();
			void collectTopmostElements(Element* element);

			Layout* owner()
				{ return this; }

		private:
			Element* activeElement(const PointerInputInfo& p);
			Element* getActiveElement(const PointerInputInfo& p, Element* e);
			void setCurrentElement(const PointerInputInfo& p, Element* e);
			void addElementToRenderQueue(Element* element, RenderContext* rc, GuiRenderer& gr);

		private:
			Element* _currentElement;
			Element* _focusedElement;
			Element* _capturedElement;
			Element::List _topmostElements;
			vec2 _dragInitialPosition;
			bool _valid;
			bool _dragging;
		};
	}
}
