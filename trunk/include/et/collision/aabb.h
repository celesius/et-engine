#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	enum AABBCorner
	{ 
		AABBCorner_LeftUpFar,
		AABBCorner_RightUpFar,
		AABBCorner_LeftDownFar,
		AABBCorner_RightDownFar,
		AABBCorner_LeftUpNear,
		AABBCorner_RightUpNear,
		AABBCorner_LeftDownNear,
		AABBCorner_RightDownNear,
		AABBCorner_max
	};

	static const AABBCorner AABBCorner_First = AABBCorner_LeftUpFar;

	struct AABB
	{
		typedef vec4 AABBCorners[AABBCorner_max];

		vec3 center;
		vec3 dimension;
		AABBCorners corners; 

		AABB() { }
		AABB(const vec3& aCenter, const vec3& aDimension);

		inline vec3 minVertex() const
			{ return center - dimension; }

		inline vec3 maxVertex() const
			{ return center + dimension; }
	};

}