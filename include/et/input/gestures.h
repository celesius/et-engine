/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/properties.h>
#include <et/input/input.h>

namespace et
{
	class GesturesRecognizer : private InputHandler
	{
	public:
		GesturesRecognizer();

	public:
		ET_DECLARE_PROPERTY(float, scrollZoomScale, setScrollZoomScale)
		ET_DECLARE_PROPERTY(float, doubleClickThreshold, setDoubleClickThreshold)
		ET_DECLARE_PROPERTY(float, holdThreshold, setHoldThreshold)

	public:
		ET_DECLARE_EVENT1(zoom, float)
		ET_DECLARE_EVENT1(drag, vec2)
		ET_DECLARE_EVENT0(pressed)
		ET_DECLARE_EVENT0(click)
		ET_DECLARE_EVENT0(doubleClick)
		ET_DECLARE_EVENT0(hold)
		ET_DECLARE_EVENT0(released)
		ET_DECLARE_EVENT0(cancelled)

	private:
		void onPointerPressed(et::PointerInputInfo);
		void onPointerMoved(et::PointerInputInfo);
		void onPointerReleased(et::PointerInputInfo);
        void onPointerCancelled(et::PointerInputInfo);
		void onPointerScrolled(et::PointerInputInfo);

		void handlePointersMovement();

	private:
		struct PointersInputDelta
		{
			PointerInputInfo current;
			PointerInputInfo previous;

			PointersInputDelta() 
				{ } 

			PointersInputDelta(const PointerInputInfo& c, const PointerInputInfo& p) : 
				current(c), previous(p) { }
		};
        typedef std::map<size_t, PointersInputDelta> PointersInputDeltaMap;

		PointersInputDeltaMap _pointers;
	};
}