#pragma once

#include <et/geometry/matrix4.h>

namespace et
{
	template <typename T>
	struct Triangle
	{
		vector3<T> v1;
		vector3<T> v2;
		vector3<T> v3;

	public:
		Triangle() { }
		Triangle(const vector3<T>& av1, const vector3<T>& av2, const vector3<T>& av3) : v1(av1), v2(av2), v3(av3) { }

		vector3<T> edge21() const 
			{ return v2 - v1; }

		vector3<T> edge31() const
			{ return v3 - v1; }

		vector3<T> normal() const
			{ return cross(edge21(), edge31()); }

		vector3<T> normalizedNormal() const
			{ return normalize(normal()); }

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

		T square()
		{
			T _one = static_cast<T>(1);
			matrix3<T> sx(vector3<T>(_one, v1.y, v1.z), vector3<T>(_one, v2.y, v2.z), vector3<T>(_one, v3.y, v3.z));
			matrix3<T> sy(vector3<T>(v1.x, _one, v1.z), vector3<T>(v2.x, _one, v2.z), vector3<T>(v3.x, _one, v3.z));
			matrix3<T> sz(vector3<T>(v1.x, v1.y, _one), vector3<T>(v2.x, v2.y, _one), vector3<T>(v3.x, v3.y, _one));
			return vector3<T>(sx.determinant(), sy.determinant(), sz.determinant()).length();
		}
	};

}