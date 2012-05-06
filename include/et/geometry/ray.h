/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

namespace et
{
	template <typename T>
	struct Ray
	{
	public:
		vector3<T> origin;
		vector3<T> direction;

	public:
		Ray(const vector3<T>& o, const vector3<T>& d) : origin(o), direction(d) { }
	};

}