#pragma once

#include <et/gui/element2d.h>

namespace et
{
	namespace gui
	{
		class FullscreenElement : public Element2D
		{
		public:
			typedef IntrusivePtr<FullscreenElement> Pointer;

		public:
			FullscreenElement(et::RenderContext* rc, Element* parent) : 
				Element2D(parent)
			{
				setFlag(ElementFlag_TransparentForPointer);
				layout(rc->size());
			}

			void layout(const vec2& sz)
			{
				setFrame(vec2(0.0f), sz);
				layoutChildren();
			}
		};
	}
}