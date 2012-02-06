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