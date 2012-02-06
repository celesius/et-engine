#pragma once 

#include <et/geometry/geometry.h>

namespace et
{
	struct OBB
	{
		vec3 center;
		vec3 dimension;
		mat3 transform;

		OBB() 
			{ };

		OBB(const vec3& c, const vec3& d) : 
			center(c), dimension(d), transform(IDENTITY_MATRIX3) { }

		OBB(const vec3& c, const vec3& d, const mat3& t) : 
			center(c), dimension(d), transform(t) { }
	};
}