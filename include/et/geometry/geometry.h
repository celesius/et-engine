#include <math.h>
#include <et/geometry/vector2.h>
#include <et/geometry/vector3.h>
#include <et/geometry/vector4.h>
#include <et/geometry/matrix3.h>
#include <et/geometry/matrix4.h>
#include <et/geometry/quaternion.h>
#include <et/geometry/splines.h>
#include <et/geometry/rect.h>
#include <et/geometry/parallelepiped.h>

#pragma once

namespace et
{
	typedef vector2<float> vec2;
	typedef vector3<float> vec3;
	typedef vector4<float> vec4;

	typedef vector2<int> vec2i;
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

	typedef Ce2PartialyQualifiedSpline<vec3, float> Ce2Vec3PartialyQualifiedSpline;
	typedef Ce2FullyQualifiedSpline<vec3, float> Ce2Vec3FullyQualifiedSpline;
	typedef Ce2SplineSequence<vec3, float> Ce2Vec3SplineSequence;

	template <typename T>
	struct Ray_T
	{
		vector3<T> origin;
		vector3<T> direction;
		Ray_T(const vector3<T>& o, const vector3<T>& d) : origin(o), direction(d) { }
	};

	template <typename T>
	struct Segment_T
	{
		vector3<T> start;
		vector3<T> end;

	public:
		Segment_T(const vec3& s, const vec3& e) : start(s), end(e) { }

	public:
		vector3<T> direction() const { return normalize(end - start); }
	};

	template <typename T>
	struct Triangle_T
	{
		vector3<T> v1;
		vector3<T> v2;
		vector3<T> v3;

	public:
		Triangle_T() { }
		Triangle_T(const vector3<T>& av1, const vector3<T>& av2, const vector3<T>& av3) : v1(av1), v2(av2), v3(av3) { }

		vector3<T> edge21() const 
			{ return v2 - v1; }

		vector3<T> edge31() const
			{ return v3 - v1; }

		vector3<T> normalizedNormal() const
			{ return normalize(cross(edge21(), edge31())); }

		vector3<T> normal() const
			{ return cross(edge21(), edge31()); }

		const vector3<T>& operator[](int i) const
		{
			if (i == 1) return v2;
			if (i == 2) return v3;
			return v1;
		}

		vector3<T>& operator[](int i)
		{
			if (i == 1) return v2;
			if (i == 2) return v3;
			return v1;
		}
	};

	typedef Ray_T<float> Ray;
	typedef Segment_T<float> Segment;
	typedef Triangle_T<float> Triangle;

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

	template<typename format>
	inline vector2<format> absv(const vector2<format>& value) 
		{ return vector2<format>(::abs(value.x), ::abs(value.y)); }

	template<typename format>
	inline vector3<format> absv(const vector3<format>& value) 
		{ return vector3<format>(::abs(value.x), ::abs(value.y), ::abs(value.z)); }

	template<typename format>
	inline vector4<format> absv(const vector4<format>& value) 
		{ return vector4<format>(::abs(value.x), ::abs(value.y), ::abs(value.z), ::abs(value.w)); }

	template <typename T>
	inline T etMin(const T& v1, const T& v2)
		{ return (v1 < v2) ? v1 : v2; }
	
	template <typename T>
	inline T etMax(const T& v1, const T& v2)
		{ return (v1 > v2) ? v1 : v2; }

	template<typename format>
	inline vector2<format> maxv(const vector2<format>& v1, const vector2<format>& v2) 
		{ return vector2<format>(etMax(v1.x, v2.x), etMax(v1.y, v2.y)); }

	template<typename format>
	inline vector3<format> maxv(const vector3<format>& v1, const vector3<format>& v2) 
		{ return vector3<format>(etMax(v1.x, v2.x), etMax(v1.y, v2.y), etMax(v1.z, v2.z)); }

	template<typename format>
	inline vector4<format> maxv(const vector4<format>& v1, const vector4<format>& v2) 
		{ return vector4<format>(etMax(v1.x, v2.x), etMax(v1.y, v2.y), etMax(v1.z, v2.z), etMax(v1.w, v2.w)); }

	template<typename format>
	inline vector2<format> minv(const vector2<format>& v1, const vector2<format>& v2) 
		{ return vector2<format>(etMin(v1.x, v2.x), etMin(v1.y, v2.y)); }

	template<typename format>
	inline vector3<format> minv(const vector3<format>& v1, const vector3<format>& v2) 
		{ return vector3<format>(etMin(v1.x, v2.x), etMin(v1.y, v2.y), etMin(v1.z, v2.z)); }

	template<typename format>
	inline vector4<format> minv(const vector4<format>& v1, const vector4<format>& v2) 
		{ return vector4<format>(etMin(v1.x, v2.x), etMin(v1.y, v2.y), etMin(v1.z, v2.z), etMin(v1.w, v2.w)); }

	template <typename T>
	inline vector2<T> floorv(const vector2<T>& v)
		{ return vector2<T>(::floor(v.x), ::floor(v.y)); }

	template <typename T>
	inline vector3<T> floorv(const vector3<T>& v)
		{ return vector3<T>(::floor(v.x), ::floor(v.y), ::floor(v.z)); }

	template <typename T>
	inline vector4<T> floorv(const vector4<T>& v)
		{ return vector4<T>(::floor(v.x), ::floor(v.y), ::floor(v.z), ::floor(v.w)); }
	
	template <typename vecType, typename genType>
	inline vecType minv(vecType& v1, vecType& v2)
	{
		vecType value;
		genType* ptr = value.raw();
		genType* pt0 = v1.raw();
		genType* pt1 = v2.raw();
		for (int i = 0; i < sizeof(vecType) / sizeof(genType); ++i)
		{
			*ptr = *pt0 < *pt1 ? *pt0 : *pt1;
			ptr++;
			pt0++;
			pt1++;
		}
		return value;
	}

	inline float sign(float s)
		{ return (s == 0.0f) ? 0.0f : s / fabs(s); }

	inline float sign_nz(float s)
		{ return (s < 0.0f) ? -1.0f : 1.0f; }

	inline vec3 randVector(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f)
	{
		return vec3(-sx + 2.0f * sx * rand() / RAND_MAX, 
					-sy + 2.0f * sy * rand() / RAND_MAX, 
					-sz + 2.0f * sz * rand() / RAND_MAX );
	}

	inline float randf(float low = -1.0f, float up = 1.0f)
	{
		return low + (up - low) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	template<typename T>
	inline T clamp(T value, T min, T max)
	{ return (value < min) ? min : (value > max) ? max : value; }

	inline std::pair<float, int> min3(float a, float b, float c)
	{
		int k = 0; 
		float m = a;
		if (b < m) {k = 1; m = b;}
		if (c < m) {k = 2; m = c;}
		return std::pair<float, int>(m, k);
	}

	inline vec3ub vec3fto3ubscaled(const vec3 &fv)
	{
		return vec3ub(
			static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.x, 0.0, 1.0) * 255.0f), 
			static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.y, 0.0, 1.0) * 255.0f),
			static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255.0f));

	}

	inline vec3ub vec3fto3ublinear(const vec3& fv)
	{
		return vec3ub(
			static_cast<unsigned char>(clamp(fv.x * 255.0f, 0.0f, 255.0f)), 
			static_cast<unsigned char>(clamp(fv.y * 255.0f, 0.0f, 255.0f)),
			static_cast<unsigned char>(clamp(fv.z * 255.0f, 0.0f, 255.0f)));

	}

	inline vec4ub vec4f_to_4ub(const vec4 &fv)
	{
		return vec4ub( (unsigned char)(clamp<float>(0.5f + 0.5f * fv.x, 0.0, 1.0) * 255), 
			(unsigned char)(clamp<float>(0.5f + 0.5f * fv.y, 0.0, 1.0) * 255), 
			(unsigned char)(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255),
			(unsigned char)(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255) );
	}

	inline mat4 translationMatrix(float x, float y, float z)
		{ return _transMatrix<float>(x, y, z); }

	inline mat4 scaleMatrix(float x, float y, float z)
		{ return _scaleMatrix<float>(x, y, z); }

	inline mat4 rotationYXZMatrix(float x, float y, float z)
		{ return _rotationYXZMatrix<float>(x, y, z); }

	inline mat4 translationMatrix(const vec3& v)
		{ return _transMatrix<float>(v.x, v.y, v.z); }

	inline mat4 translationScaleMatrix(const vec3& t, const vec3& s)
		{ return _transScaleMatrix<float>(t.x, t.y, t.z, s.x, s.y, s.z); }

	inline mat4 scaleMatrix(const vec3& v) 
		{ return _scaleMatrix<float>(v.x, v.y, v.z); }

	inline mat4 rotationYXZMatrix(const vec3& v) 
		{ return _rotationYXZMatrix<float>(v.x, v.y, v.z); }

	inline mat4 rotationScaleMatrix(const vec3& r, const vec3& s) 
		{ return _rotationScaleMatrix<float>(r.x, r.y, r.z, s.x, s.y, s.z); }

	inline mat4 translationRotationYXZMatrix(const vec3& t, const vec3& r) 
		{ return _translationRotationYXZMatrix<float>(t.x, t.y, t.z, r.x, r.y, r.z); }

	inline mat4 orientationForNormal(const vec3& n) 
		{ return _orientationForNormal<float>(n); }

	inline mat4 rotation2DMatrix(float angle)
	{	
		float ca = cos(angle);
		float sa = sin(angle);
		mat4 m;
		m[0][0] = ca; m[0][1] = -sa;
		m[1][0] = sa; m[1][1] =  ca;
		m[2][2] = m[3][3] = 1.0f;
		return m;
	}

	inline mat4 transform2DMatrix(float a, const vec2& scale, const vec2& translate)
	{
		float ca = cos(a);
		float sa = sin(a);
		mat4 m;
		m[0][0] = ca * scale.x; m[0][1] = -sa * scale.y;
		m[1][0] = sa * scale.x; m[1][1] =  ca * scale.y;
		m[2][2] = 1.0f;
		m[3][0] = translate.x;
		m[3][1] = translate.y;
		m[3][3] = 1.0f;
		return m;
	}
	
	inline mat3 rotation2DMatrix3(float angle)
	{	
		float ca = cos(angle);
		float sa = sin(angle);
		mat3 m;
		m[0][0] = ca; m[0][1] = -sa;
		m[1][0] = sa; m[1][1] =  ca;
		m[2][2] = m[3][3] = 1.0f;
		return m;
	}

	inline mat4 transformYXZMatrix(vec3 translate, vec3 rotate) 
		{ return _transformYXZMatrix<float>(translate.x, translate.y, translate.z, rotate.x, rotate.y, rotate.z); }

	template<typename format>
	inline vector3<format> planeNormal(const vector3<format>& p0, const vector3<format>& p1, const vector3<format>& p2)
		{ return normalize(cross(p1 - p0, p2 - p0)); }

	inline float mix(float v1, float v2, float t)
		{ return v1 * (1.0f - t) + v2 * t; }

	template<typename format>
	inline vector4<format> mix(vector4<format> v1, vector4<format> v2, format t)
	{
		format nt = 1 - t;
		return vector4<format>( v1.x * nt + v2.x * t, 
			v1.y * nt + v2.y * t, 
			v1.z * nt + v2.z * t, 
			v1.w * nt + v2.w * t);
	}

	template<typename format>
	inline vector3<format> mix(vector3<format> v1, vector3<format> v2, format t)
	{ 
		format nt = 1 - t;
		return vector3<format>(v1.x * nt + v2.x * t, 
			v1.y * nt + v2.y * t, 
			v1.z * nt + v2.z * t);
	}

	template<typename format>
	inline vector2<format> mix(vector2<format> v1, vector2<format> v2, float t)
	{ return vector2<format>(format(v1.x * (1.0f - t) + v2.x * t), format(v1.y * (1.0f - t) + v2.y * t));}

	template<typename format>
	inline vector3<format>fromSpherical(format theta, format phi)
	{
		format fCosTheta = cos(theta);
		return vec3( fCosTheta * cos(phi), sin(theta), fCosTheta * sin(phi) );
	}

	template<typename format>
	inline vector3<format>fromSphericalRotated(format theta, format phi)
	{
		format fSinTheta = sin(theta);
		return vec3( fSinTheta * cos(phi), cos(theta), fSinTheta * sin(phi) );
	}

	template <typename T>
	inline vector3<T> toSpherical(const vector3<T>& vec)
	{
		vector3<T> normalized_v = normalize(vec);
		return vector3<T>( atan2(normalized_v.z, normalized_v.x), asin(normalized_v.y), vec.length() );
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
	inline vector4<T> normalizePlane(const vector4<T>& v) 
	{ 
		T l = v.xyz().dotSelf();
		return (l > 0) ? v / ::sqrt(l) : vector4<T>(0);
	}

	template <typename format>
	inline vector3<format> cross(const vector3<format>& v1, const vector3<format>& v2) 
	{ return v1.cross(v2); }

	template <typename format>
	inline format length(const vector3<format>& v)
	{ return v.length(); }

	template <typename T>
	inline T dot(const vector2<T>& v1, const vector2<T>& v2) 
		{ return v1.x*v2.x + v1.y*v2.y; }

	template <typename T>
	inline T dot(const vector3<T>& v1, const vector3<T>& v2) 
		{ return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }

	template <typename T> 
	inline vector3<T> reflect(const vector3<T>& v, const vector3<T>& n)
		{ return v - static_cast<T>(2) * dot(v, n) * n; }

	template <typename format>
	inline std::ostream& operator << (std::ostream& stream, const vector2<format>& value) 
	{ 
		stream << value.x << CSV_DELIMITER << value.y;
		return stream;
	}

	template <typename format>
	inline std::ostream& operator << (std::ostream& stream, const vector3<format>& value) 
	{ 
		stream << value.x << CSV_DELIMITER << value.y << CSV_DELIMITER << value.z;
		return stream;
	}

	template <typename format>
	inline std::ostream& operator << (std::ostream& stream, const vector4<format>& value) 
	{ 
		stream << value.x << CSV_DELIMITER << value.y << CSV_DELIMITER << value.z << CSV_DELIMITER << value.w;
		return stream;
	}

	template<typename format>
	inline format sqr(format value) 
		{ return value*value; }

	inline size_t getIndex(size_t u, size_t v, size_t u_sz, size_t v_sz)
		{ return clamp<size_t>(u, 0, u_sz - 1) + clamp<size_t>(v, 0, v_sz - 1) * u_sz; }

	template <typename format>
	inline format triangleSquare(const vector3<format>& v1, const vector3<format>& v2, const vector3<format>& v3)
	{
		matrix3<format>sx;
		sx[0] = vector3<format>(1, v1.y, v1.z);
		sx[1] = vector3<format>(1, v2.y, v2.z);
		sx[2] = vector3<format>(1, v3.y, v3.z);

		matrix3<format>sy;
		sy[0] = vector3<format>(v1.x, 1, v1.z);
		sy[1] = vector3<format>(v2.x, 1, v2.z);
		sy[2] = vector3<format>(v3.x, 1, v3.z);

		matrix3<format>sz;
		sz[0] = vector3<format>(v1.x, v1.y, 1);
		sz[1] = vector3<format>(v2.x, v2.y, 1);
		sz[2] = vector3<format>(v3.x, v3.y, 1);

		vector3<format>s (sx.determinant(), sy.determinant(), sz.determinant());

		return s.length();
	}

	inline vec2 operator * (const mat4& m, const vec2& v)
	{ 
			return vec2(
				m[0][0] * v.x + m[1][0] * v.y + m[3][0],
				m[0][1] * v.x + m[1][1] * v.y + m[3][1] );
	}

	template <typename T>
	Triangle_T<T> operator * (const matrix4<T>& m, const Triangle_T<T>& t)
	{
		return Triangle_T<T>(m * t.v1, m * t.v2, m * t.v3);
	}

	quaternion matrixToQuaternion(const mat3& m);
	void decomposeMatrix(mat4 mat, vec3& translation, quaternion& rotation, vec3& scale);
	vec3 removeMatrixScale(mat3& m);
	vec3 removeMatrixScale(mat4& m);
	vec3 removeMatrixScaleRowMajor(mat3& mat);
}