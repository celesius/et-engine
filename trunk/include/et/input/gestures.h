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
		ET_DECLARE_PROPERTY(float, doubleClickThreshold, setÂoubleClickThreshold)
		ET_DECLARE_PROPERTY(float, holdThreshold, setÐoldThreshold)

	public:
		ET_DECLARE_EVENT1(zoom, float)
		ET_DECLARE_EVENT1(drag, vec2)
		ET_DECLARE_EVENT0(pressed)
		ET_DECLARE_EVENT0(click)
		ET_DECLARE_EVENT0(doubleClick)
		ET_DECLARE_EVENT0(hold)
		ET_DECLARE_EVENT0(released)

	private:
		void onPointerPressed(et::PointerInputInfo);
		void onPointerMoved(et::PointerInputInfo);
		void onPointerReleased(et::PointerInputInfo);
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

		std::map<size_t, PointersInputDelta> _pointers;
	};
}