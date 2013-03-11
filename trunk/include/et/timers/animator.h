/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/properties.h>
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
		BaseAnimator(const TimerPool& tp) : 
			_tag(0), _delegate(0), _timerPool(tp) { }

		ET_DECLARE_PROPERTY_SETTER_COPY(AnimatorDelegate*, delegate, setDelegate)
		ET_DECLARE_PROPERTY(int, tag, setTag)

	protected:
		BaseAnimator(AnimatorDelegate* delegate, int tag, const TimerPool& tp) : 
			 _tag(tag), _delegate(delegate), _timerPool(tp) { }

		TimerPool& timerPool()
			{ return _timerPool; }

	protected:
		TimerPool _timerPool;
	};

	typedef std::vector<BaseAnimator*> AnimatorList;

	template <typename T>
	class Animator : public BaseAnimator
	{
	public:
		Animator(const TimerPool& tp) :
			BaseAnimator(0, 0, tp) { }

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

			*value = from;

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
				delegate()->animatorUpdated(this);

				cancelUpdates();
				delegate()->animatorFinished(this);
			}
			else 
			{
				*_value = _from * (1.0f - dt) + _to * dt;
				delegate()->animatorUpdated(this);
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