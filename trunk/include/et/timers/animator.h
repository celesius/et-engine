#pragma once

#include <et/app/events.h>
#include <et/timers/timedobject.h>

namespace et
{

	class BaseAnimator;
	class AnimatorDelegate
	{
	public:
		virtual ~AnimatorDelegate() { }
		virtual void animatorUpdated(BaseAnimator*) = 0;
		virtual void animatorFinished(BaseAnimator*) = 0;
	};

	class BaseAnimator : public TimedObject
	{
	public:
		int tag;

	public:
		BaseAnimator(const TimerPool& tp) : _timerPool(tp)
			{ }

		void cancel() 
		{ 
			cancelUpdates(); 
			_delegate->animatorFinished(this);
		}

	protected:
		BaseAnimator(AnimatorDelegate* delegate, int t, const TimerPool& tp) : 
			 tag(t), _delegate(delegate), _timerPool(tp) { }

		TimerPool& timerPool()
			{ return _timerPool; }

	protected:
		AnimatorDelegate* _delegate;
		TimerPool _timerPool;
	};

	typedef std::vector<BaseAnimator*> AnimatorList;

	template <typename T>
	class Animator : public BaseAnimator
	{
	public:
		Animator(AnimatorDelegate* delegate, int tag, const TimerPool& tp) : 
			BaseAnimator(delegate, tag, tp) { }

		Animator(AnimatorDelegate* delegate, T* value, const T& from, const T& to, float duration, int tag, const TimerPool& tp) : 
			BaseAnimator(delegate, tag, tp)
		{
			animate(value, from, to, duration);
		}

		void animate(T* value, const T& from, const T& to, float duration)
		{
			startUpdates(timerPool().ptr());

			_from = from;
			_to = to;
			_value = value;
			_duration = duration;
			_startTime = actualTime();
		};


	protected:
		void update(float t)
		{
			float dt = (t - _startTime) / _duration;
			if (dt >= 1.0f)
			{
				*_value = _to;
				_delegate->animatorUpdated(this);
				cancel();
			}
			else 
			{
				*_value = _from * (1.0f - dt) + _to * dt;
				_delegate->animatorUpdated(this);
			}
		}

	private:
		T _from;
		T _to;
		T* _value;
		float _startTime;
		float _duration;
	};

	typedef Animator<float> FloatAnimator;

}