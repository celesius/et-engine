#pragma once

#include <et/geometry/vector3.h>

namespace et
{
	template <typename T>
	struct Segment
	{
		vector3<T> start;
		vector3<T> end;

	public:
		Segment(const vector3<T>& s, const vector3<T>& e) : start(s), end(e) { }

		vector3<T> direction() const 
			{ return normalize(end - start); }
	};
}