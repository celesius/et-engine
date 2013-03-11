/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/geometry/matrix4.h>

namespace et
{
	template <typename T>
	class Triangle
	{
	public:
		Triangle() { }

		Triangle(const vector3<T>& av1, const vector3<T>& av2, const vector3<T>& av3) : 
			_v1(av1), _v2(av2), _v3(av3) { fillSupportData(); }

		Triangle(vector3<T>&& av1, vector3<T>&& av2, vector3<T>&& av3) : 
			_v1(av1), _v2(av2), _v3(av3) { fillSupportData(); }

		const vector3<T>& v1() const 
			{ return _v1; }

		const vector3<T>& v2() const
			{ return _v2; }

		const vector3<T>& v3() const
			{ return _v3; }

		const vector3<T>& edge2to1() const 
			{ return _edge2to1; }

		const vector3<T>& edge3to1() const
			{ return _edge3to1; }

		const vector3<T>& edge3to2() const
			{ return _edge3to2; }

		const vector3<T>& normalizedNormal() const
			{ return _normal; }

		T square()
		{
			T _aOne = static_cast<T>(1);
			matrix3<T> sx(vector3<T>(_aOne, _v1.y, _v1.z), vector3<T>(_aOne, _v2.y, _v2.z), vector3<T>(_aOne, _v3.y, _v3.z));
			matrix3<T> sy(vector3<T>(_v1.x, _aOne, _v1.z), vector3<T>(_v2.x, _aOne, _v2.z), vector3<T>(_v3.x, _aOne, _v3.z));
			matrix3<T> sz(vector3<T>(_v1.x, _v1.y, _aOne), vector3<T>(_v2.x, _v2.y, _aOne), vector3<T>(_v3.x, _v3.y, _aOne));
			return vector3<T>(sx.determinant(), sy.determinant(), sz.determinant()).length();
		}

	private:
		
		void fillSupportData()
		{
			_edge2to1 = _v2 - _v1;
			_edge3to1 = _v3 - _v1;
			_edge3to2 = _v3 - _v2;
			_normal = normalize(cross(_edge2to1, _edge3to1));
		}

	private:
		vector3<T> _v1;
		vector3<T> _v2;
		vector3<T> _v3;
		vector3<T> _normal;
		vector3<T> _edge2to1;
		vector3<T> _edge3to1;
		vector3<T> _edge3to2;
	};

}