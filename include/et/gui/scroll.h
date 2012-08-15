/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once 

#include <et/gui/Element2d.h>

namespace et
{
	namespace gui
	{
		class Scroll : public Element2d
		{
		public:
			typedef IntrusivePtr<Scroll> Pointer;
			
		public:
			Scroll(Element2d* parent);
			void addToRenderQueue(RenderContext*, GuiRenderer&);
			
			mat4 finalTransform();
			mat4 finalInverseTransform();
			
			bool pointerPressed(const PointerInputInfo&);			
			bool pointerMoved(const PointerInputInfo&);			
			bool pointerReleased(const PointerInputInfo&);
			bool pointerCancelled(const PointerInputInfo&);
			bool containsPoint(const vec2& p, const vec2& np);
			
			const vec2& contentSize() const
				{ return _contentSize; }
			
			void setContentSize(const vec2& cs);
			void adjustContentSize();
			
		private:
			void invalidateChildren();
			void broadcastPressed(const PointerInputInfo&);
			void broadcastMoved(const PointerInputInfo&);
			void broadcastReleased(const PointerInputInfo&);
			void broadcastCancelled(const PointerInputInfo&);
			
			void update(float t);
			void applyOffset(const vec2& dOffset);
			
		private:
			PointerInputInfo _currentPointer;
			PointerInputInfo _previousPointer;
			vec2 _contentSize;
			vec2 _offset;
			vec2 _velocity;
			float _updateTime;
			bool _pointerCaptured;
			bool _manualScrolling;
		};

	}
}