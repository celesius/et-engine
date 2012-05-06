/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/app/events.h>
#include <et/geometry/geometry.h>

namespace et
{
	struct AccelerometerData
	{
		vec3 value;
		float timestamp;

		AccelerometerData() : 
			timestamp(0.0f) { }

		AccelerometerData(const vec3& v) : 
			value(v), timestamp(0.0f) { }

		AccelerometerData(const vec3& v, float t) : 
			value(v), timestamp(t) { }
	};

	struct GyroscopeData
	{
		vec3 rate;
		float timestamp;

		GyroscopeData() : 
			timestamp(0.0f) { }

		GyroscopeData(const vec3& r) : 
			rate(r), timestamp(0.0f) { }

		GyroscopeData(const vec3& r, float t) : 
			rate(r), timestamp(t) { }
	};

	class OrientationManagerPrivate;
	class OrientationManager
	{
	public:
		static bool accelerometerAvailable();
		static bool gyroscopeAvailable();

	public:
		OrientationManager();
		~OrientationManager();

		void setAccelerometerEnabled(bool e);
		bool accelerometerEnabled(bool e) const;

		void setGyroscopeEnabled(bool e);
		bool gyroscopeEnabled(bool e) const;

		ET_DECLARE_EVENT1(accelerometerDataUpdated, AccelerometerData)
		ET_DECLARE_EVENT1(gyroscopeDataUpdated, GyroscopeData)

	private:
		OrientationManagerPrivate* _private;
	};
}