#pragma once

#include <et/geometry/vector2.h>
#include <et/geometry/vector3.h>

namespace et 
{

	template <typename T>
	struct vector4
	{
		union
		{
			struct { T x, y, z, w; };
			T c[4];
		};

		vector4() : x(0), y(0), z(0), w(0)
			{ }

		vector4(const vector4& c) : x(c.x), y(c.y), z(c.z), w(c.w)
			{ }

		vector4(vector4&& c) : x(c.x), y(c.y), z(c.z), w(c.w)
			{ }
		
		explicit vector4(T s) : x(s), y(s), z(s), w(s)
			{ }

		vector4(const vector2<T>& v1, const vector2<T>& v2) : x(v1.x), y (v1.y), z(v2.x), w(v2.y)
			{ }

		vector4(vector3<T> v3, T _w) : x(v3.x), y(v3.y), z(v3.z), w(_w)
			{ }

		explicit vector4(vector3<T> v3) : x(v3.x), y(v3.y), z(v3.z), w(0)
			{ }

		vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w)
			{ }

		T* data()
			{ return c; }

		const T* data() const 
			{ return c; }

		char* binary()
			{ return reinterpret_cast<char*>(c); }

		const char* binary() const
			{ return reinterpret_cast<const char*>(c); }

		T& operator[](int i)
			{ return c[i] ;}

		const T& operator[](int i) const 
			{ return c[i] ;}

		vector4 operator -()
			{ return vector4<T>(-x, -y, -z, -w); }

		const vector4 operator -() const
			{ return vector4<T>(-x, -y, -z, -w); }

		vector4 operator * (T v) const
			{ return vector4(x * v, y * v, z * v, w * v); }

		vector4 operator / (T v) const
			{ return vector4(x / v, y / v, z / v, w / v); }

		vector4 operator + (const vector4& value) const
			{ return vector4(x + value.x, y + value.y, z + value.z, w + value.w); }

		vector4 operator - (const vector4& value) const
			{ return vector4(x - value.x, y - value.y, z - value.z, w - value.w); }

		vector4 operator * (const vector4& value) const
			{ return vector4(x * value.x, y * value.y, z * value.z, w * value.w); }

		vector4 operator / (const vector4& value) const
			{ return vector4(x / value.x, y / value.y, z / value.z, w / value.w); }

		vector4& operator += (const vector4 &value)
			{ x += value.x; y += value.y; z += value.z; w += value.w; return *this; }

		vector4& operator -= (const vector4 &value)
			{ x -= value.x; y -= value.y; z -= value.z; w -= value.w; return *this; }

		vector4& operator *= (const vector4 &value)
			{ x *= value.x; y *= value.y; z *= value.z; w *= value.w; return *this; }

		vector4& operator /= (const vector4 &value)
			{ x /= value.x; y /= value.y; z /= value.z; w /= value.w; return *this; }

		vector4& operator *= (T value)
			{ x *= value; y *= value; z *= value; w *= value; return *this; }

		vector4& operator /= (T value)
			{ x /= value; y /= value; z /= value; w /= value; return *this; }
		
		vector4& operator = (const vector4& value)
			{ x = value.x; y = value.y; z = value.z; w = value.w; return *this; }
		
		vector4& operator = (vector4&& value)
			{ x = value.x; y = value.y; z = value.z; w = value.w; return *this; }

		vector3<T>& xyz()
			{ return *(vector3<T>*)c; }

		const vector3<T>& xyz() const
			{ return *(vector3<T>*)c; }

		vector2<T>& xy()
			{ return *(vector2<T>*)c; }

		const vector2<T>& xy() const
			{ return *(vector2<T>*)c; }

		T dot(const vector4<T>& vector) const
			{ return x*vector.x + y*vector.y + z*vector.z + w*vector.w; }

		T dotSelf() const
			{ return x*x + y*y + z*z + w*w; }
	};

	template <typename T>
	inline vector4<T> operator * (T value, const vector4<T>& vec) 
		{ return vector4<T>(vec.x * value, vec.y * value, vec.z * value, vec.w * value); }

	template <typename T>
	inline vector4<T> sqrtv(const vector4<T>& v)
		{ return vector4<T>(::sqrt(v.x), ::sqrt(v.y), ::sqrt(v.z), ::sqrt(v.w)); }

}