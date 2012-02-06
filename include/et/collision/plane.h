#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	class Plane
	{
	public:
		vec4 equation;

		Plane(const vec4& eq) : equation(eq)
			{ } 

		Plane(const vec3& normal, float distance) : equation(normal, distance)
			{ }

		Plane(const Triangle& t) : equation(planeNormal(t.v1, t.v2, t.v3), 0.0)
			{ equation.w = dot(equation.xyz(), t.v1); }

		const vec3& normal() const 
			{ return equation.xyz(); }

		float distance() const 
			{ return equation.w; }

		vec3 planePoint() const
			{ return equation.xyz() * equation.w; }

		vec3 projectionOfPoint(const vec3& pt) const
			{ return pt - equation.xyz() * distanceToPoint(pt); }

		float distanceToPoint(const vec3& pt) const
			{ return dot(equation.xyz(), pt) - equation.w; }
	};

}