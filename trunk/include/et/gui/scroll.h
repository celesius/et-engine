/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once 

#include <et/gui/element2d.h>

namespace et
{
	namespace gui
	{
		class Scroll : public Element2D
		{
		public:
			typedef IntrusivePtr<Scroll> Pointer;
			
		public:
			Scroll(Element2D* parent);
			void addToRenderQueue(RenderContext*, GuiRenderer&);
			
			mat4 finalTransform();
			mat4 finalInverseTransform();
			
			bool pointerPressed(const PointerInputInfo&);			
			bool pointerMoved(const PointerInputInfo&);			
			bool pointerReleased(const PointerInputInfo&);
			
			bool containsPoint(const vec2& p, const vec2& np);
			
		private:
			void invalidateChildren();
			void broadcastPressed(const PointerInputInfo&);
			void broadcastReleased(const PointerInputInfo&);
			void broadcastCanceled(const PointerInputInfo&);
			
		private:
			vec2 _offset;
			PointerInputInfo _dragPoint;
			bool _dragging;
		};

	}
}