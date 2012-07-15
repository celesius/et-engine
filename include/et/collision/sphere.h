/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	struct Sphere
	{
	public:
		Sphere() : 
			_radius(0.0f) { }

		Sphere(const vec3& aCenter, float aRadius) :
			_center(aCenter), _radius(aRadius) { } 

		vec3 center() 
			{ return _center; }

		const vec3& center() const
			{ return _center; }

		float radius() const
			{ return _radius; }
		
		void setRadius(float r)
			{ _radius = r; }

	protected:
		vec3 _center;
		float _radius;

	};
}