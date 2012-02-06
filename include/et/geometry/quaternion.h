#pragma once

#include <et/core/constants.h>
#include <et/geometry/vector3.h>
#include <et/geometry/vector4.h>
#include <et/geometry/matrix4.h>

namespace et
{

	template <typename T>
	struct Quaternion
	{
		T scalar;
		vector3<T> vector;

		Quaternion() : scalar(static_cast<T>(1)), vector(static_cast<T>(0)) { }
		Quaternion(const vector3<T>& v) : scalar(static_cast<T>(0)), vector(v) { }
		Quaternion(T s, T x, T y, T z) : scalar(s), vector(x, y, z) { }
		Quaternion(T angle, const vector3<T>& axis)
		{
			T half = angle  / static_cast<T>(2);
			scalar = static_cast<T>(cos(half));
			vector = static_cast<T>(sin(half)) * axis;
		}

		T& operator[](int i) 
			{ return *(&scalar + i); }

		const T& operator[](int i) const
			{ return *(&scalar + i); }

		Quaternion operator !() { return Quaternion(scalar, -vector); }

		Quaternion operator + (const Quaternion &q)
			{ return Quaternion(scalar+q.scalar, vector+q.vector); }

		Quaternion operator - (const Quaternion &q)
			{ return Quaternion(scalar-q.scalar, vector-q.vector); }

		Quaternion operator * (const Quaternion &q) const
		{
			Quaternion result;
			result.scalar = scalar * q.scalar - dot(vector, q.vector);
			result.vector = vector.cross(q.vector) + scalar * q.vector + q.scalar * vector;
			return result;
		};

		Quaternion operator * (T v) const
		{
			Quaternion result;
			result.scalar = scalar * v;
			result.vector = vector * v;
			return result;
		};

		Quaternion operator / (T v) const
		{
			Quaternion result;
			result.scalar = scalar / v;
			result.vector = vector / v;
			return result;
		};

		T length()
			{ return sqrt(scalar*scalar + vector.dotSelf()); }

		Quaternion normalize() 
		{ 
			T len = this->length();
			return (len > 0) ? *this / length() : Quaternion<T>();
		}

		vector3<T> transform(vector3<T> &v)
		{
			Quaternion& thisOne = *this;
			return (thisOne * Quaternion(v) * !thisOne).vector;
		}

		vector3<T> invtransform(vector3<T> &v)
		{
			Quaternion& thisOne = *this;
			return (!thisOne * Quaternion(v) * thisOne).vector;
		}

		Quaternion<T>& operator += (const Quaternion &q)
		{ 
			scalar = +q.scalar; 
			vector += q.vector; 
			return *this;
		}

		Quaternion<T>& operator -= (const Quaternion &q) 
		{
			scalar = -q.scalar; 
			vector -= q.vector; 
			return *this;
		}

		Quaternion<T>& operator *= (const Quaternion &q) 
		{
			T s = scalar * q.scalar - dot(vector, q.vector);
			vector3<T> v = vector.cross(q.vector) + scalar * q.vector + q.scalar * vector;
			scalar = s;
			vector = v;
			return *this;
		}

		matrix4<T> toMatrix() const
		{
			T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

			x2 = vector.x + vector.x;
			y2 = vector.y + vector.y;
			z2 = vector.z + vector.z;
			xx = vector.x * x2; xy = vector.x * y2; xz = vector.x * z2;
			yy = vector.y * y2; yz = vector.y * z2; zz = vector.z * z2;
			wx = scalar * x2; wy = scalar * y2; wz = scalar * z2;

			return matrix4<T>(
				vector4<T>(1.0f - (yy+zz), xy-wz , xz+wy , 0.0),
				vector4<T>(xy+wz , 1.0f - (xx+zz), yz-wx , 0.0),
				vector4<T>(xz-wy , yz+wx , 1.0f - (xx+yy), 0.0),
				vector4<T>(0.0 , 0.0 , 0.0 , 1.0));
		}
	};

	template <typename T>
	inline Quaternion<T> operator * (T value, const Quaternion<T>& q)
		{ return q * value; }
}