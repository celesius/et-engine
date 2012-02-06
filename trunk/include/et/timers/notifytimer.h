#pragma once

#include <et/app/events.h>
#include <et/timers/timedobject.h>

namespace et
{
	class NotifyTimer : public TimedObject
	{
	public:
		enum RepeatCountFlag
		{
			Repeat_Forever = -1,
			DontRepear = 0
		};

	public:
		NotifyTimer();

		void start(TimerPool& tp, float period, int repeatCount = DontRepear);
		void update(float t);

		ET_DECLARE_EVENT1(expired, NotifyTimer*)

	private:
		float _endTime;
		float _period;
		int _repeatCount;
	};
}