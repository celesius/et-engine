#include <et/collision/collision.h>

using namespace et;

float et::distanceFromPointToLine(const vec3& p, const vec3& l0, const vec3& l1, vec3& projection)
{
	vec3 v = p - l0;
	vec3 s = l1 - l0;
	vec3 disp = s * (dot(v, s) / s.dotSelf());
	projection = l0 + disp;
	v -= disp;
	return v.length();
}

vec2 et::barycentricCoordinates(const vec3& p, const Triangle& t)
{
	vec3 v0 = t.edge31();
	vec3 v1 = t.edge21();
	vec3 v2 = p - t.v1;
	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	return vec2((dot11 * dot02 - dot01 * dot12) * invDenom, (dot00 * dot12 - dot01 * dot02) * invDenom);
}

vec3 et::worldCoordinatesFromBarycentric(const vec2& b, const Triangle& t)
{
	return t.v1 * (1.0f - b.x - b.y) + t.v2 * b.y + t.v3 * b.x;
}

vec3 et::closestPointOnTriangle(const vec3& sourcePosition, const Triangle& triangle)
{
	vec3 edge0 = triangle[1] - triangle[0];
	vec3 edge1 = triangle[2] - triangle[0];
	vec3 v0 = triangle[0] - sourcePosition;

	float a = dot(edge0, edge0 );
	float b = dot(edge0, edge1 );
	float c = dot(edge1, edge1 );
	float d = dot(edge0, v0 );
	float e = dot(edge1, v0 );

	float det = a*c - b*b;
	float s = b*e - c*d;
	float t = b*d - a*e;

	if ( s + t < det )
	{
		if ( s < 0.f )
		{
			if ( t < 0.f )
			{
				if ( d < 0.f )
				{
					s = clamp( -d/a, 0.f, 1.f );
					t = 0.f;
				}
				else
				{
					s = 0.f;
					t = clamp( -e/c, 0.f, 1.f );
				}
			}
			else
			{
				s = 0.f;
				t = clamp( -e/c, 0.f, 1.f );
			}
		}
		else if ( t < 0.f )
		{
			s = clamp( -d/a, 0.f, 1.f );
			t = 0.f;
		}
		else
		{
			float invDet = 1.f / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		if ( s < 0.f )
		{
			float tmp0 = b+d;
			float tmp1 = c+e;
			if ( tmp1 > tmp0 )
			{
				float numer = tmp1 - tmp0;
				float denom = a-2*b+c;
				s = clamp( numer/denom, 0.f, 1.f );
				t = 1-s;
			}
			else
			{
				t = clamp( -e/c, 0.f, 1.f );
				s = 0.f;
			}
		}
		else if ( t < 0.f )
		{
			if ( a+d > b+e )
			{
				float numer = c+e-b-d;
				float denom = a-2*b+c;
				s = clamp( numer/denom, 0.f, 1.f );
				t = 1-s;
			}
			else
			{
				s = clamp( -e/c, 0.f, 1.f );
				t = 0.f;
			}
		}
		else
		{
			float numer = c+e-b-d;
			float denom = a-2*b+c;
			s = clamp( numer/denom, 0.f, 1.f );
			t = 1.f - s;
		}
	}

	return triangle[0] + s * edge0 + t * edge1;
/*
	vec3 ab = t.edge21();
	vec3 ac = t.edge31();
    vec3 ap = p - t.v1;

    float d1 = dot(ab, ap);
    float d2 = dot(ac, ap);
	if ((d1 <= 0.0f) && (d2 <= 0.0f))
		return t.v1;

    vec3 bp = p - t.v2;
    float d3 = dot(ab, bp);
    float d4 = dot(ac, bp);
    if ((d3 >= 0.0f) && (d4 <= d3))
		return t.v2;

    float vc = d1 * d4 - d3 * d2;
    if ((vc <= 0.0f) && (d1 >= 0.0f) && (d3 <= 0.0f))
		return t.v1 + ab * (d1 / (d1 - d3));

    vec3 cp = p - t.v3;
    float d5 = dot(ab, cp);
    float d6 = dot(ac, cp);
    if ((d6 >= 0.0f) && (d5 <= d6))
		return t.v3;

    float vb = d5 * d2 - d1 * d6;
    if ((vb <= 0.0f) && (d2 >= 0.0f) && (d6 <= 0.0f))
        return t.v1 + ac * d2 / (d2 - d6);

    float va = d3 * d6 - d5 * d4;
    if ((va <= 0.0f) && ((d4 - d3) >= 0.0f) && ((d5 - d6) >= 0.0f))
        return t.v2 + (t.v3 - t.v2) * (d4 - d3) / ((d4 - d3) + (d5 - d6));

    float denom = 1.0f / (va + vb + vc);
    return t.v1 + ab * vb * denom + ac * vc * denom;
*/
}


bool et::pointInsideTriangle(const vec3& p, const Triangle& t)
{
	vec2 b = barycentricCoordinates(p, t);
	return (b.x >= 0) && (b.y >= 0) && (b.x + b.y <= 1.0f);
}

bool et::intersect::raySphere(const Ray& r, const Sphere& s, vec3* intersection_pt)
{
	vec3 dv = r.origin - s.center();
	float b = 2.0f * dot(r.direction, dv);
	float c = dv.dotSelf() - sqr(s.radius());
	float d = b * b - 4.0f * c;

	if (d < 0.0) return false;

	if (intersection_pt)
	{
		float sqrt_d = ::sqrt(d);
		float t0 = sqrt_d - b;
		float t1 = -b - sqrt_d;
		float t = (t0 < t1) ? t0 : t1;
		*intersection_pt = r.origin + 0.5f * t * r.direction;
	}

	return true;
}

bool et::intersect::rayPlane(const Ray& r, const Plane& p, vec3* intersection_pt)
{
	float d = dot(r.direction, p.normal());
	if (d >= 0.0f) return false;

	float t = dot(p.normal(), p.planePoint() - r.origin) / d;

	if (t <= 0.0) 
		return false;

	if (intersection_pt)
		*intersection_pt = r.origin + t * r.direction;

	return true;
}

bool et::intersect::rayTriangle(const Ray& r, const Triangle& t, vec3* intersection_pt)
{
	vec3 ip;
	if (!rayPlane(r, Plane(t), &ip)) return false;

	if (pointInsideTriangle(ip, t))
	{
		if (intersection_pt)
			*intersection_pt = ip;
		return true;
	}
	else 
		return false;
}

bool et::intersect::segmentPlane(const Segment& s, const Plane& p, vec3* intersection_pt)
{
	vec3 ds = s.end - s.start;
	float d = dot(ds, p.normal());
	if (d >= 0.0f) return false;

	float t = dot(p.normal(), p.planePoint() - s.start) / d;
	if ((t <= 0.0f) || (t > 1.0f)) return false;

	if (intersection_pt)
		*intersection_pt = s.start + t * ds;

	return true;
}

bool et::intersect::segmentTriangle(const Segment& s, const Triangle& t, vec3* intersection_pt)
{
	vec3 ip;
	if (!segmentPlane(s, Plane(t), &ip)) return false;

	if (pointInsideTriangle(ip, t))
	{
		if (intersection_pt)
			*intersection_pt = ip;
		return true;
	}

	return false;
}

bool et::intersect::triangleTriangle(const et::Triangle& t0, const et::Triangle& t1)
{
#define ProjectOntoAxis(triangle, axis, fmin, fmax) \
	dot1 = dot(axis, triangle.v2);					\
	dot2 = dot(axis, triangle.v3);					\
	fmin = dot(axis, triangle.v1), fmax = fmin;		\
	if (dot1 < fmin) fmin = dot1; else if (dot1 > fmax) fmax = dot1; \
	if (dot2 < fmin) fmin = dot2; else if (dot2 > fmax) fmax = dot2;

	float min0 = 0.0f;
	float min1 = 0.0f;
	float max0 = 0.0f;
	float max1 = 0.0f;
	float dot1 = 0.0f;
	float dot2 = 0.0f;

	vec3 E0[3] = 
	{
		(t0.v2 - t0.v1).normalize(), 
		(t0.v3 - t0.v2).normalize(), 
		(t0.v1 - t0.v3).normalize()
	};
	vec3 N0 = cross(E0[0], E0[1]);
	float N0dT0V0 = dot(N0, t0.v1);
	ProjectOntoAxis(t1, N0, min1, max1);
	if ((N0dT0V0 < min1) || (N0dT0V0 > max1)) return false;

	vec3 E1[3] = 
	{
		(t1.v2 - t1.v1).normalize(), 
		(t1.v3 - t1.v2).normalize(), 
		(t1.v1 - t1.v3).normalize()
	};
	vec3 N1 = cross(E1[0], E1[1]);
	if (cross(N0, N1).dotSelf() >= 1.0e-5)
	{
		float N1dT1V0 = dot(N1, t1.v1);
		ProjectOntoAxis(t0, N1, min0, max0);
		if ((N1dT1V0 < min0) || (N1dT1V0 > max0)) return false;

		for (int i = 0; i < 3; ++i)
		{
			vec3 dir = cross(E0[0], E1[i]);
			ProjectOntoAxis(t0, dir, min0, max0);
			ProjectOntoAxis(t1, dir, min1, max1);
			if ((max0 < min1) || (max1 < min0)) return false;
			dir = cross(E0[1], E1[i]);
			ProjectOntoAxis(t0, dir, min0, max0);
			ProjectOntoAxis(t1, dir, min1, max1);
			if ((max0 < min1) || (max1 < min0)) return false;
			dir = cross(E0[2], E1[i]);
			ProjectOntoAxis(t0, dir, min0, max0);
			ProjectOntoAxis(t1, dir, min1, max1);
			if ((max0 < min1) || (max1 < min0)) return false;
		}
	}
	else 
	{
		for (int i = 0; i < 3; ++i)
		{
			vec3 dir = cross(N0, E0[i]);
			ProjectOntoAxis(t0, dir, min0, max0);
			ProjectOntoAxis(t1, dir, min1, max1);
			if ((max0 < min1) || (max1 < min0)) return false;
			dir = cross(N1, E1[i]);
			ProjectOntoAxis(t0, dir, min0, max0);
			ProjectOntoAxis(t1, dir, min1, max1);
			if ((max0 < min1) || (max1 < min0)) return false;
		}

	}

#undef ProjectOntoAxis
	return true;
}

bool et::intersect::sphereSphere(const Sphere& s1, const Sphere& s2, vec3* amount)
{
	vec3 dv = s2.center() - s1.center();

	float distance = dv.length();
	float radiusSum = s1.radius() + s2.radius();
	bool collised = (distance <= radiusSum);

	if (amount)
		*amount = dv.normalize() * (radiusSum - distance);

	return collised;
}

bool et::intersect::sphereBox(const vec3& sphereCenter, float sphereRadius, const vec3& boxCenter, const vec3& boxExtent)
{
	vec3 bMin = boxCenter - boxExtent;
	vec3 bMax = boxCenter + boxExtent;

	float d = 0; 
	if (sphereCenter.x < bMin.x) d += sqr(sphereCenter.x - bMin.x); 
	else if (sphereCenter.x > bMax.x) d += sqr(sphereCenter.x - bMax.x); 
	if (sphereCenter.y < bMin.y) d += sqr(sphereCenter.y - bMin.y); 
	else if (sphereCenter.y > bMax.y) d += sqr(sphereCenter.y - bMax.y); 
	if (sphereCenter.z < bMin.z) d += sqr(sphereCenter.z - bMin.z); 
	else if (sphereCenter.z > bMax.z) d += sqr(sphereCenter.z - bMax.z); 
	return d <= sqr(sphereRadius); 
}

bool et::intersect::sphereAABB(const Sphere& s, const AABB& b)
{
	return sphereBox(s.center(), s.radius(), b.center, b.dimension);
}

bool et::intersect::sphereOBB(const Sphere& s, const OBB& b)
{
	return sphereBox(b.center + b.transform.transpose() * (s.center() - b.center), s.radius(), b.center, b.dimension);
}

bool et::intersect::sphereTriangle(const vec3& sphereCenter, const float sphereRadius, const Triangle& t, vec3* normal, float* penetration)
{
	Plane p(t);
	if (p.distanceToPoint(sphereCenter) > sphereRadius) return false;

	vec3 direction = closestPointOnTriangle(sphereCenter, t) - sphereCenter;
	float distance = direction.dotSelf();
	if (distance > sqr(sphereRadius)) return false;

	if (normal)
		*normal = p.normal();

	if (penetration)
		*penetration = sphereRadius - sqrtf(distance);

	return true;
}

bool et::intersect::sphereTriangle(const Sphere& s, const Triangle& t, vec3* normal, float* penetration)
{
	return sphereTriangle(s.center(), s.radius(), t, normal, penetration);
}

bool et::intersect::sphereTriangle(const Sphere& s, const vec3& sphereVelocity, const Triangle& t, vec3* normal, 
	float* penetration, float* intersectionTime)
{
	Plane p(t);
	if (p.distanceToPoint(s.center()) <= s.radius())
		 return sphereTriangle(s, t, normal, penetration);

	vec3 triangleNormal = p.normal();
	float NdotV = dot(triangleNormal, sphereVelocity);
	if (NdotV >= 0.0f) return false;

	if (intersectionTime)
	{
		*intersectionTime = (s.radius() + p.equation.w - dot(triangleNormal, s.center())) / NdotV;
		vec3 movedCenter = s.center() + *intersectionTime * sphereVelocity;
		return sphereTriangle(movedCenter, s.radius(), t, normal, penetration);
	}

	return false;
}

bool et::intersect::sphereTriangles(const Sphere& s, const Triangle* triangles, const size_t triangleCount, vec3* normal, float* penetration)
{
	for (size_t i = 0; i < triangleCount; ++i)
	{
		if (sphereTriangle(s.center(), s.radius(), triangles[i], normal, penetration))
			return true;
	}
	return false;
}

bool et::intersect::sphereTriangles(const Sphere& s, const vec3& sphereVelocity, const Triangle* triangles, const size_t triangleCount, 
								vec3* normal, float* penetration, float* intersectionTime)
{
	for (size_t i = 0; i < triangleCount; ++i)
	{
		if (sphereTriangle(s, sphereVelocity, triangles[i], normal, penetration, intersectionTime))
			return true;
	}

	return false;
}