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
	class GesturesRecognizer : private InputHandler, private TimedObject
	{
	public:
		GesturesRecognizer(bool automaticMode = true);

	public:
		ET_DECLARE_PROPERTY(float, clickThreshold, setClickThreshold)
		ET_DECLARE_PROPERTY(float, doubleClickThreshold, setDoubleClickThreshold)
		ET_DECLARE_PROPERTY(float, holdThreshold, setHoldThreshold)

	public:
		ET_DECLARE_EVENT1(zoom, float)
		ET_DECLARE_EVENT1(scroll, vec2)

		ET_DECLARE_EVENT2(pressed, vec2, PointerType)
		ET_DECLARE_EVENT2(drag, vec2, PointerType)
		ET_DECLARE_EVENT2(moved, vec2, PointerType)
		ET_DECLARE_EVENT2(released, vec2, PointerType)
		ET_DECLARE_EVENT2(click, vec2, PointerType)
		ET_DECLARE_EVENT2(doubleClick, vec2, PointerType)

		ET_DECLARE_EVENT0(hold)
		ET_DECLARE_EVENT0(cancelled)

		ET_DECLARE_EVENT1(pointerPressed, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerMoved, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerReleased, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerCancelled, PointerInputInfo)

	public:
		void onPointerPressed(PointerInputInfo);
		void onPointerMoved(PointerInputInfo);
		void onPointerReleased(PointerInputInfo);
        void onPointerCancelled(PointerInputInfo);
		void onPointerScrolled(PointerInputInfo);

		void onGesturePerformed(GestureInputInfo);

	private:
		void update(float);
		void handlePointersMovement();
		void startWaitingForClicks();
		void stopWaitingForClicks();
		void cancelWaitingForClicks();

		GesturesRecognizer(const GesturesRecognizer&) : InputHandler(false)
			{ }
		
		GesturesRecognizer& operator = (const GesturesRecognizer&)
			{ return *this; }

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
		PointerType _singlePointerType;
		vec2 _singlePointerPosition;
		float _actualTime;
		float _clickStartTime;
		bool _expectClick;
		bool _expectDoubleClick;
		bool _clickTimeoutActive;
	};
}