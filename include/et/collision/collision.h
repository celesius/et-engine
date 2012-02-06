#pragma once

#include <et/collision/plane.h>
#include <et/collision/sphere.h>
#include <et/collision/aabb.h>
#include <et/collision/obb.h>

namespace et
{
	namespace intersect
	{
		bool sphereSphere(const Sphere& s1, const Sphere& s2, vec3* amount);
		bool sphereBox(const vec3& sphereCenter, float sphereRadius, const vec3& boxCenter, const vec3& boxExtent);
		bool sphereAABB(const Sphere& s, const AABB& b);
		bool sphereOBB(const Sphere& s, const OBB& b);

		bool sphereTriangle(const vec3& sphereCenter, const float sphereRadius, const Triangle& t, vec3* normal, 
			float* penetration);
		bool sphereTriangle(const Sphere& s, const Triangle& t, vec3* normal,
			float* penetration);
		bool sphereTriangles(const Sphere& s, const Triangle* triangles, const size_t triangleCount, vec3* normal, 
			float* penetration);

		bool sphereTriangle(const Sphere& s, const vec3& sphereVelocity, const Triangle& t, vec3* normal, 
			float* penetration, float* intersectionTime);
		bool sphereTriangles(const Sphere& s, const vec3& sphereVelocity, const Triangle* triangles, const size_t triangleCount, vec3* normal,
			float* penetration, float* intersectionTime);

		bool raySphere(const Ray& r, const Sphere& s, vec3* intersection_pt);
		bool rayPlane(const Ray& r, const Plane& p, vec3* intersection_pt);
		bool rayTriangle(const Ray& r, const Triangle& t, vec3* intersection_pt);

		bool segmentPlane(const Segment& s, const Plane& p, vec3* intersection_pt);
		bool segmentTriangle(const Segment& s, const Triangle& t, vec3* intersection_pt);

		bool triangleTriangle(const Triangle& t1, const Triangle& t2);

	}

	vec2 barycentricCoordinates(const vec3& p, const Triangle& t);
	vec3 worldCoordinatesFromBarycentric(const vec2& b, const Triangle& t);
	vec3 closestPointOnTriangle(const vec3& p, const Triangle& t);

	bool pointInsideTriangle(const vec3& p, const Triangle& t);

	float distanceFromPointToLine(const vec3& p, const vec3& l0, const vec3& l1, vec3& projection);
}