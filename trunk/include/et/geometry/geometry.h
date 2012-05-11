/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <stdlib.h>
#include <math.h>
#include <et/geometry/vector4.h>
#include <et/geometry/matrix3.h>
#include <et/geometry/matrix4.h>
#include <et/geometry/ray.h>
#include <et/geometry/triangle.h>
#include <et/geometry/rect.h>
#include <et/geometry/segment.h>
#include <et/geometry/plane.h>
#include <et/geometry/quaternion.h>
#include <et/geometry/parallelepiped.h>

namespace et
{
	typedef vector2<float> vec2;
	typedef vector3<float> vec3;
	typedef vector4<float> vec4;
	typedef vector2<int> vec2i;
	typedef vector2<size_t> vec2sz;
	typedef vector3<int> vec3i;
	typedef vector4<int> vec4i;
	typedef vector3<unsigned char> vec3ub;
	typedef vector4<unsigned char> vec4ub;
	typedef matrix3<float> mat3;
	typedef matrix4<float> mat4;
	typedef matrix3<int> mat3i;
	typedef matrix4<int> mat4i;
	typedef Quaternion<float> quaternion;
	typedef Rect<float> rect;
	typedef Rect<int> recti;
	typedef Parallelepiped<float> parallelepiped;
	typedef Ray<float> ray;
	typedef Segment<float> segment;
	typedef Triangle<float> triangle;
	typedef Plane<float> plane;

	static const vec3 nullVec3(0.0f);
	static const vec3 unitX(1.0f, 0.0f, 0.0f);
	static const vec3 unitY(0.0f, 1.0f, 0.0f);
	static const vec3 unitZ(0.0f, 0.0f, 1.0f);

	static const mat3 IDENTITY_MATRIX3( 
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f));

	static const mat4 IDENTITY_MATRIX(
		vec4(1.0f, 0.0f, 0.0f, 0.0f), 
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f) );

	static const mat4 MATRIX_LIGHT_PROJECTION(
		vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f) );

	template<typename T>
	inline T sqr(T value) 
		{ return value*value; }

	template<typename T>
	inline vector2<T> absv(const vector2<T>& value) 
		{ return vector2<T>(::fabs(value.x), ::fabs(value.y)); }

	template<typename T>
	inline vector3<T> absv(const vector3<T>& value) 
		{ return vector3<T>(::fabs(value.x), ::fabs(value.y), ::fabs(value.z)); }

	template<typename T>
	inline vector4<T> absv(const vector4<T>& value) 
		{ return vector4<T>(::fabs(value.x), ::fabs(value.y), ::fabs(value.z), ::fabs(value.w)); }

	inline vector2<int> absv(const vector2<int>& value) 
		{ return vector2<int>(::abs(value.x), ::abs(value.y)); }
	
	inline vector3<int> absv(const vector3<int>& value) 
		{ return vector3<int>(::abs(value.x), ::abs(value.y), ::abs(value.z)); }
	
	inline vector4<int> absv(const vector4<int>& value) 
		{ return vector4<int>(::abs(value.x), ::abs(value.y), ::abs(value.z), ::abs(value.w)); }
	
	template <typename T>
	inline T etMin(const T& v1, const T& v2)
		{ return (v1 < v2) ? v1 : v2; }
	
	template <typename T>
	inline T etMax(const T& v1, const T& v2)
		{ return (v1 > v2) ? v1 : v2; }

	template<typename T>
	inline vector2<T> maxv(const vector2<T>& v1, const vector2<T>& v2) 
		{ return vector2<T>(etMax(v1.x, v2.x), etMax(v1.y, v2.y)); }

	template<typename T>
	inline vector3<T> maxv(const vector3<T>& v1, const vector3<T>& v2) 
		{ return vector3<T>(etMax(v1.x, v2.x), etMax(v1.y, v2.y), etMax(v1.z, v2.z)); }

	template<typename T>
	inline vector4<T> maxv(const vector4<T>& v1, const vector4<T>& v2) 
		{ return vector4<T>(etMax(v1.x, v2.x), etMax(v1.y, v2.y), etMax(v1.z, v2.z), etMax(v1.w, v2.w)); }

	template<typename T>
	inline vector2<T> minv(const vector2<T>& v1, const vector2<T>& v2) 
		{ return vector2<T>(etMin(v1.x, v2.x), etMin(v1.y, v2.y)); }

	template<typename T>
	inline vector3<T> minv(const vector3<T>& v1, const vector3<T>& v2) 
		{ return vector3<T>(etMin(v1.x, v2.x), etMin(v1.y, v2.y), etMin(v1.z, v2.z)); }

	template<typename T>
	inline vector4<T> minv(const vector4<T>& v1, const vector4<T>& v2) 
		{ return vector4<T>(etMin(v1.x, v2.x), etMin(v1.y, v2.y), etMin(v1.z, v2.z), etMin(v1.w, v2.w)); }

	template <typename T>
	inline vector2<T> floorv(const vector2<T>& v)
		{ return vector2<T>(::floor(v.x), ::floor(v.y)); }

	template <typename T>
	inline vector3<T> floorv(const vector3<T>& v)
		{ return vector3<T>(::floor(v.x), ::floor(v.y), ::floor(v.z)); }

	template <typename T>
	inline vector4<T> floorv(const vector4<T>& v)
		{ return vector4<T>(::floor(v.x), ::floor(v.y), ::floor(v.z), ::floor(v.w)); }

	template <typename T>
	inline vector4<T> sqrtv(const vector4<T>& v)
		{ return vector4<T>(::sqrt(v.x), ::sqrt(v.y), ::sqrt(v.z), ::sqrt(v.w)); }
	
	template <typename T>
	inline vector3<T> sqrtv(const vector3<T>& v)
		{ return vector3<T>(::sqrt(v.x), ::sqrt(v.y), ::sqrt(v.z)); }
	
	template <typename T>
	inline vector2<T> sqrtv(const vector2<T>& v)
		{ return vector2<T>(::sqrt(v.x), ::sqrt(v.y)); }

	template<typename T>
	inline T clamp(T value, T min, T max)
		{ return (value < min) ? min : (value > max) ? max : value; }

	inline float mix(float v1, float v2, float t)
		{ return v1 * (1.0f - t) + v2 * t; }

	template<typename T>
	inline vector4<T> mix(vector4<T> v1, vector4<T> v2, T t)
	{
		T nt = 1 - t;
		return vector4<T>(v1.x * nt + v2.x * t, v1.y * nt + v2.y * t, v1.z * nt + v2.z * t, v1.w * nt + v2.w * t);
	}

	template<typename T>
	inline vector3<T> mix(vector3<T> v1, vector3<T> v2, T t)
	{ 
		T nt = 1 - t;
		return vector3<T>(v1.x * nt + v2.x * t, v1.y * nt + v2.y * t, v1.z * nt + v2.z * t);
	}

	template<typename T>
	inline vector2<T> mix(vector2<T> v1, vector2<T> v2, float t)
	{
		float nt = static_cast<T>(1) - t;
		return vector2<T>(T(v1.x * nt + v2.x * t), T(v1.y * nt + v2.y * t));
	}

	template<typename T>
	inline vector3<T>fromSpherical(T theta, T phi)
	{
		T fCosTheta = cos(theta);
		return vec3(fCosTheta * cos(phi), sin(theta), fCosTheta * sin(phi));
	}

	template<typename T>
	inline vector3<T>fromSphericalRotated(T theta, T phi)
	{
		T fSinTheta = sin(theta);
		return vec3(fSinTheta * cos(phi), cos(theta), fSinTheta * sin(phi));
	}

	template <typename T>
	inline vector3<T> toSpherical(const vector3<T>& vec)
	{
		vector3<T> normalized_v = normalize(vec);
		return vector3<T>(atan2(normalized_v.z, normalized_v.x), asin(normalized_v.y), vec.length());
	}

	template <typename T>
	inline vector2<T> normalize(const vector2<T>& v) 
	{ 
		T l = v.dotSelf();
		return (l > 0) ? v / ::sqrt(l) : vector2<T>(0);
	}

	template <typename T>
	inline vector3<T> normalize(const vector3<T>& v) 
	{ 
		T l = v.dotSelf();
		return (l > 0) ? v / ::sqrt(l) : vector3<T>(0);
	}

	template <typename T>
	inline vector4<T> normalize(const vector4<T>& v) 
	{ 
		T l = v.dotSelf();
		return (l > 0) ? v / ::sqrt(l) : vector4<T>(0);
	}

	template <typename T>
	inline Quaternion<T> normalize(const Quaternion<T>& q) 
	{ 
		T l = q.length();
		return (l > 0) ? q / ::sqrt(l) : Quaternion<T>();
	}

	template <typename T>
	inline vector4<T> normalizePlane(const vector4<T>& v) 
	{ 
		T l = v.xyz().dotSelf();
		return (l > 0) ? v / ::sqrt(l) : vector4<T>(0);
	}

	template <typename T>
	inline vector3<T> cross(const vector3<T>& v1, const vector3<T>& v2) 
		{ return v1.cross(v2); }

	template <typename T>
	inline T dot(const vector2<T>& v1, const vector2<T>& v2) 
		{ return v1.x*v2.x + v1.y*v2.y; }

	template <typename T>
	inline T dot(const vector3<T>& v1, const vector3<T>& v2) 
		{ return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }

	template <typename T> 
	inline vector3<T> reflect(const vector3<T>& v, const vector3<T>& n)
		{ return v - static_cast<T>(2) * dot(v, n) * n; }

	template <typename T>
	Triangle<T> operator * (const matrix3<T>& m, const Triangle<T>& t)
		{ return Triangle<T>(m * t.v1, m * t.v2, m * t.v3); }

	template <typename T>
	Triangle<T> operator * (const matrix4<T>& m, const Triangle<T>& t)
		{ return Triangle<T>(m * t.v1, m * t.v2, m * t.v3); }

	template <typename T>
	inline matrix4<T> translationMatrix(T x, T y, T z)
	{
		matrix4<T> M;
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = T(1);
		M[3][0] = x;
		M[3][1] = y;
		M[3][2] = z;
		return M;
	}

	template <typename T>
	inline matrix4<T> translationScaleMatrix(T tx, T ty, T tz, T sx, T sy, T sz)
	{
		matrix4<T> M;
		M[0][0] = sx;
		M[1][1] = sy;
		M[2][2] = sz;
		M[3][3] = T(1);

		M[3][0] = tx;
		M[3][1] = ty;
		M[3][2] = tz;
		return M;
	}

	template <typename T>
	inline matrix4<T> scaleMatrix(T x, T y, T z)
	{
		matrix4<T> M;
		M[0][0] = x;
		M[1][1] = y;
		M[2][2] = z;
		M[3][3] = T(1);
		return M;
	}

	template <typename T>
	inline matrix4<T> rotationYXZMatrix(T x, T y, T z)
	{
		matrix4<T> m(static_cast<T>(1));

		float sx = sin(x);
		float cx = cos(x);
		float sy = sin(y);
		float cy = cos(y);
		float sz = sin(z);
		float cz = cos(z);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           

		return m;
	}

	template <typename T>
	inline matrix4<T> translationRotationYXZMatrix(T tx, T ty, T tz, T rx, T ry, T rz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
		m[3][0] =  tx;				 m[3][1] =  ty;    m[3][2] = tz;
		m[3][3] = 1;

		return m;
	}

	template <typename T>
	inline matrix4<T> rotationScaleMatrix(T rx, T ry, T rz, T scx, T scy, T scz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] = scx * (cz*cy - sz*sx*sy); 
		m[0][1] = scy * (-cx*sz); 
		m[0][2] = scz * (cz*sy + sz*sx*cy);

		m[1][0] = scx * (sz*cy + cz*sx*sy);
		m[1][1] = scy * (cx*cz); 
		m[1][2] = scz * (sz*sy - cz*sx*cy);

		m[2][0] = scx * (-cx*sy);
		m[2][1] = scy * (sx);
		m[2][2] = scz * (cx*cy);
		m[3][3] = 1;

		return m;
	}

	template <typename T>
	inline matrix4<T> transformYXZMatrix(T tx, T ty, T tz, T rx, T ry, T rz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
		m[3][0] = tx;                m[3][1] =  ty;    m[3][2] = tz; 

		m[3][3] = 1;

		return m;
	}
	
	template <typename T>
	inline matrix4<T> orientationForNormal(const vector3<T>& n) 
	{
		vector3<T> up = normalize(n);
		T theta = asin(up.y) - static_cast<T>(1.570796326795);
		T phi = atan2(up.z, up.x) + static_cast<T>(1.570796326795);
		T csTheta = cos(theta);
		vector3<T> side2(csTheta * cos(phi), sin(theta), csTheta * sin(phi));
		vector3<T> side1 = up.cross(side2);
		
		matrix4<T> result;
		result[0].xyz() = vector3<T>(side1.x, up.x, side2.x);
		result[1].xyz() = vector3<T>(side1.y, up.y, side2.y);
		result[2].xyz() = vector3<T>(side1.z, up.z, side2.z);
		return result;
	}	

	inline mat4 translationMatrix(const vec3& v)
		{ return translationMatrix<float>(v.x, v.y, v.z); }

	inline mat4 translationScaleMatrix(const vec3& t, const vec3& s)
		{ return translationScaleMatrix<float>(t.x, t.y, t.z, s.x, s.y, s.z); }

	inline mat4 scaleMatrix(const vec3& v) 
		{ return scaleMatrix<float>(v.x, v.y, v.z); }

	inline mat4 rotationYXZMatrix(const vec3& v) 
		{ return rotationYXZMatrix<float>(v.x, v.y, v.z); }

	inline mat4 rotationScaleMatrix(const vec3& r, const vec3& s) 
		{ return rotationScaleMatrix<float>(r.x, r.y, r.z, s.x, s.y, s.z); }

	inline mat4 translationRotationYXZMatrix(const vec3& t, const vec3& r) 
		{ return translationRotationYXZMatrix<float>(t.x, t.y, t.z, r.x, r.y, r.z); }

	inline mat4 transformYXZMatrix(vec3 translate, vec3 rotate) 
		{ return transformYXZMatrix<float>(translate.x, translate.y, translate.z, rotate.x, rotate.y, rotate.z); }

	inline vec2 operator * (const mat4& m, const vec2& v)
		{ return vec2(m[0][0] * v.x + m[1][0] * v.y + m[3][0], m[0][1] * v.x + m[1][1] * v.y + m[3][1] ); }

	inline vec2i operator * (const mat4i& m, const vec2i& v)
		{ return vec2i(m[0][0] * v.x + m[1][0] * v.y + m[3][0], m[0][1] * v.x + m[1][1] * v.y + m[3][1]); }
	
	quaternion matrixToQuaternion(const mat3& m);
	void decomposeMatrix(mat4 mat, vec3& translation, quaternion& rotation, vec3& scale);
	vec3 removeMatrixScale(mat3& m);
	vec3 removeMatrixScale(mat4& m);
	vec3 removeMatrixScaleRowMajor(mat3& mat);

	vec3 randVector(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	float randf(float low = -1.0f, float up = 1.0f);
	float sign(float s);
	float sign_nz(float s);
	vec3ub vec3fto3ubscaled(const vec3 &fv);
	vec3ub vec3fto3ublinear(const vec3& fv);
	vec4ub vec4f_to_4ub(const vec4 &fv);

	mat4 rotation2DMatrix(float angle);
	mat4 transform2DMatrix(float a, const vec2& scale, const vec2& translate);
	mat3 rotation2DMatrix3(float angle);
}