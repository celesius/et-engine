#include <math.h>
#include <et/timers/sequence.h>

using namespace et;

Sequence::Sequence(float duration, float from, float to, Curve curve ) : 
	_duration(duration), _from(from), _to(to), _dt(to - from), _curve(curve)
{

}

void Sequence::start(TimerPool& tp)
{
	startUpdates(tp.ptr());

	_startTime = actualTime();
	_endTime = _startTime + _duration;
	_time = 0.0;
}

void Sequence::update(float t)
{
	_time = t - _startTime;

	if (_time >= _endTime)
	{
		cancelUpdates();
		updated.invoke(_to);
		finished.invoke(this);
	}
	else
	{
		float normalizedTime = _time / _duration;

		if (_curve == EasyIn)
			normalizedTime = ::sqrtf(normalizedTime);
		else if (_curve == EasyOut)
			normalizedTime *= normalizedTime;

		updated.invoke(_from + _dt * normalizedTime);
	}
}
