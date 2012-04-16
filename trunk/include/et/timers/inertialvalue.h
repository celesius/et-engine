#pragma once

#include <et/timers/timedobject.h>

namespace et
{
	template <typename T>
	class InertialValue : public TimedObject
	{
	public:
		InertialValue() : 
			_velocity(0), _value(0), _deccelerationRate(1.0f), _time(0.0f) { }

		InertialValue(const T& val) :
			_velocity(0), _value(val), _deccelerationRate(1.0f), _time(0.0f) { }

		void setVelocity(const T& t)
			{ _velocity = t; }
		void addVelocity(const T& t)
			{ _velocity += t; }
		void scaleVelocity(const T& t)
			{ _velocity *= t; }

		const T& value() const
			{ return _value; }

		void setValue(const T& v)
			{ _value = v;}
		void addValue(const T& v)
			{ _value += v;}

		void run()
		{
			startUpdates(mainTimerPool().ptr());
			_time = actualTime();
		}

		InertialValue& operator = (const T& v) 
		{
			_value = v;
			return *this;
		}

		InertialValue& operator *= (const T& v)
		{
			_value *= v;
			return *this;
		}

		InertialValue& operator += (const T& v)
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
		void update(float t)
		{
			float dt = _deccelerationRate * (t - _time);
			_velocity *= etMax(0.0f, 1.0f - dt);
			_value += dt * _velocity;
			_time = t;

			valueUpdated.invoke(_value);
			updated.invoke();
		}

	private:
		T _velocity;
		T _value;
		float _deccelerationRate;
		float _time;
	};
}