/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <vector>
#include <et/geometry/geometry.h>

namespace et
{
	class RectPlacer
	{
	public:
		typedef std::vector<rect> RectList;

	public:
		RectPlacer(const vec2i& contextSize, bool addSpace);
		bool place(const vec2i& size, rect& placedPosition);

		const RectList& placedItems() const 
			{ return _placedItems; }

	private:
		vec2i _contextSize;
		RectList _placedItems;
		bool _addSpace;
	};
}