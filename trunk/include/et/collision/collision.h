/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/geometry/plane.h>
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

		bool sphereTriangle(const vec3& sphereCenter, const float sphereRadius, const triangle& t, vec3* normal, 
			float* penetration);
		bool sphereTriangle(const Sphere& s, const triangle& t, vec3* normal,
			float* penetration);
		bool sphereTriangles(const Sphere& s, const triangle* triangles, const size_t triangleCount, vec3* normal, 
			float* penetration);

		bool sphereTriangle(const Sphere& s, const vec3& sphereVelocity, const triangle& t, vec3* normal, 
			float* penetration, float* intersectionTime);
		bool sphereTriangles(const Sphere& s, const vec3& sphereVelocity, const triangle* triangles, const size_t triangleCount, vec3* normal,
			float* penetration, float* intersectionTime);

		bool rayTriangle(const ray& r, const triangle& t, vec3* intersection_pt);
		bool rayTriangles(const ray& r, const triangle* triangles, const size_t triangleCount, vec3* intersection_pt);

		bool raySphere(const ray& r, const Sphere& s, vec3* intersection_pt);
		bool rayPlane(const ray& r, const plane& p, vec3* intersection_pt);

		bool segmentPlane(const segment& s, const plane& p, vec3* intersection_pt);
		bool segmentTriangle(const segment& s, const triangle& t, vec3* intersection_pt);

		bool triangleTriangle(const triangle& t1, const triangle& t2);

	}

	vec2 barycentricCoordinates(const vec3& p, const triangle& t);
	vec3 worldCoordinatesFromBarycentric(const vec2& b, const triangle& t);
	vec3 closestPointOnTriangle(const vec3& p, const triangle& t);

	bool pointInsideTriangle(const vec3& p, const triangle& t);

	float distanceFromPointToLine(const vec3& p, const vec3& l0, const vec3& l1, vec3& projection);
}