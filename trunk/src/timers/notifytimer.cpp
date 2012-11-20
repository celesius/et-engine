/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/timers/notifytimer.h>

using namespace et;

NotifyTimer::NotifyTimer()
{
}

void NotifyTimer::start(TimerPool& tp, float period, int repeatCount)
{
	startUpdates(tp.ptr());

	_period = period;
	_repeatCount = repeatCount;
	_endTime = actualTime() + period;
}

void NotifyTimer::update(float t)
{
	if (t >= _endTime)
	{
		_repeatCount--;

		if (_repeatCount == -1)
			cancelUpdates();
		else 
			_endTime = t + _period;

		expired.invoke(this);
	}
}
