#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	struct Sphere
	{
	public:
		Sphere(const vec3& aCenter, float aRadius) : _center(aCenter), _radius(aRadius) { } 

		virtual vec3 center() 
			{ return _center; }

		virtual const vec3& center() const
			{ return _center; }

		virtual float radius() const
			{ return _radius; }
		
		virtual void setRadius(float r)
			{ _radius = r; }

	protected:
		vec3 _center;
		float _radius;

	};
}