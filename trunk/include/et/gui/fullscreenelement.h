/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/gui/element2d.h>

namespace et
{
	namespace gui
	{
		class FullscreenElement : public Element2d
		{
		public:
			typedef IntrusivePtr<FullscreenElement> Pointer;

		public:
			FullscreenElement(et::RenderContext* rc, Element* parent) : 
				Element2d(parent)
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