#pragma once

#include <et/geometry/vector2.h>

namespace et 
{
	template <typename T>
	struct vector3
	{
		union
		{
			struct { T x, y, z; };
			T c[3];
		};

		vector3() : x(0), y(0), z(0) { }
		vector3(const vector3& m) : x(m.x), y(m.y), z(m.z) { }
		vector3(vector3&& m) : x(m.x), y(m.y), z(m.z) { }
		
		explicit vector3(T val) : x(val), y(val), z(val) { }
		vector3(T i_x, T i_y) : x(i_x), y(i_y), z(0) { }
		vector3(T i_x, T i_y, T i_z) : x(i_x), y(i_y), z(i_z) { }
		vector3(vector2<T>i_xy, T i_z) : x(i_xy.x), y(i_xy.y), z(i_z) { }

		T* raw() 
			{ return c; }

		const T* raw() const
			{ return c; }

		const char* binary() const
			{ return reinterpret_cast<const char*>(c); }

		char* binary() 
			{ return reinterpret_cast<char*>(c); }

		T& operator [] (int i)
			{ return c[i]; }

		const T& operator [] (int i) const
			{ return c[i]; }

		vector3 operator -() const
			{ return vector3(-x, -y, -z); }

		vector3 operator + (const vector3& value) const
			{ return vector3(x + value.x, y + value.y, z + value.z); }

		vector3 operator - (const vector3& value) const
			{ return vector3(x - value.x, y - value.y, z - value.z); }

		vector3 operator * (const vector3& value) const
			{ return vector3(x * value.x, y * value.y, z * value.z); }

		vector3 operator / (const vector3& value) const
			{ return vector3(x / value.x, y / value.y, z / value.z); }

		vector3 operator * (const T& v) const
			{ return vector3(x * v, y * v, z * v); }

		vector3 operator / (const T& v) const
			{ return vector3(x / v, y / v, z / v); }

		vector3& operator += (const vector3 &value)
			{ x+=value.x; y+=value.y; z+=value.z; return *this; }

		vector3& operator -= (const vector3 &value)
			{ x-=value.x; y-=value.y; z-=value.z; return *this; }

		vector3& operator *= (const vector3 &value)
			{ x*=value.x; y*=value.y; z*=value.z; return *this; }

		vector3& operator /= (const vector3 &value)
			{ x/=value.x; y /=value.y; z/=value.z; return *this; }

		vector3& operator *= (T value)
			{ x*=value; y*=value; z*=value; return *this; }

		vector3& operator /= (T value)
			{ x/=value; y /= value; z /= value; return *this; }

		vector3& operator = (const vector3& value)
			{ x = value.x; y = value.y; z = value.z; return *this; }
		
		vector3& operator = (vector3&& value)
			{ x = value.x; y = value.y; z = value.z; return *this; }
		
		T dotSelf() const 
			{ return x*x + y*y + z*z; }

		T length() const 
			{ return sqrt( dotSelf() ); }

		vector3 cross(const vector3 &vec) const
			{ return vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, 	x * vec.y - y * vec.x ); 	}

		vector2<T>& xy()
			{ return *((vector2<T>*)(c)); }

		const vector2<T>& xy() const
			{ return *((vector2<T>*)(c)); }

		vector3<T>& normalize()
		{ 
			T len = dotSelf();
			if (len > 0)
			{
				len = sqrt(len);
				x /= len;
				y /= len;
				z /= len;
			}
			return *this;
		}

	};

	template <typename T>
	vector3<T> operator * (T value, const vector3<T>& vec) 
		{ return vector3<T>(vec.x * value, vec.y * value, vec.z * value); }

}