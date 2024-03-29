/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/geometry/geometry.h>
#include <et/timers/timedobject.h>
#include <et/app/events.h>

namespace et
{
	template <typename T>
	class InertialValue : public TimedObject
	{
	public:
		InertialValue() : 
			_velocity(0), _value(0), _deccelerationRate(1.0f), _time(0.0f),
			_precision(std::numeric_limits<float>::epsilon()) { }

		InertialValue(const T& val) :
			_velocity(0), _value(val), _deccelerationRate(1.0f), _time(0.0f),
			_precision(std::numeric_limits<float>::epsilon()) { }

		const T& velocity() const
			{ return _velocity; }
		
		const T& value() const
			{ return _value; }
		
		void setVelocity(const T& t)
			{ _velocity = t; }

		void addVelocity(const T& t)
			{ _velocity += t; }

		void scaleVelocity(const T& t)
			{ _velocity *= t; }

		void setValue(const T& v)
			{ _value = v;}

		void addValue(const T& v)
			{ _value += v;}

		void setPrecision(float p)
			{ _precision = p; }

		void run()
		{
			TimedObject::startUpdates(nullptr);
			_time = actualTime();
		}

		InertialValue<T>& operator = (const T& v) 
		{
			_value = v;
			return *this;
		}

		InertialValue<T>& operator *= (const T& v)
		{
			_value *= v;
			return *this;
		}

		InertialValue<T>& operator += (const T& v)
		{
			_value += v;
			return *this;
		}

		void setDeccelerationRate(float value)
			{ _deccelerationRate = etMax(0.0f, value); }

	public:
		ET_DECLARE_EVENT0(updated)
		ET_DECLARE_EVENT1(valueUpdated, const T&)

	private:

		void startUpdates(TimerPool* timerPool = nullptr) override
			{ TimedObject::startUpdates(timerPool); }

		void update(float t)
		{
			float dt = _deccelerationRate * (t - _time);
			_velocity *= etMax(0.0f, 1.0f - dt);
			
			T dValue = dt * _velocity;
			if (length(dValue) > _precision)
			{
				_value += dValue;
				valueUpdated.invoke(_value);
				updated.invoke();
			}
			
			_time = t;
		}

	private:
		T _velocity;
		T _value;
		float _deccelerationRate;
		float _time;
		float _precision;
	};
}
