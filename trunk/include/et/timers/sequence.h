#pragma once

#include <et/app/events.h>
#include <et/timers/timedobject.h>

namespace et
{
	class Sequence : public TimedObject
	{
	public:
		enum Curve
		{
			Linear,
			EasyIn,
			EasyOut
		};

		Sequence(float duration, float from = 0.0, float to = 1.0, Curve curve = Linear);

		void start(TimerPool& tp);
		void update(float t);

		ET_DECLARE_EVENT2(updated, Sequence*, float)
		ET_DECLARE_EVENT1(finished, Sequence*)

	private:
		float _startTime;
		float _endTime;
		float _duration;
		float _from;
		float _to;
		float _dt;

		Curve _curve;
	};
}