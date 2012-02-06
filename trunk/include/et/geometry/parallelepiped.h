#pragma once

namespace et
{
	template <typename T>
	struct Parallelepiped
	{
		union
		{
			struct
			{
				T left;
				T top;
				T nearValue;
				T width;
				T height;
				T depth;
			};

			T c[6];
		};

		Parallelepiped() : left(0), top(0), nearValue(0), width(0), height(0), depth(0)
			{ }

		Parallelepiped(T x, T y, T z, T w, T h, T d) : left(x), top(y), nearValue(z), width(w), height(h), depth(d)
			{ }

		Parallelepiped(const vector3<T>& pos, const vector3<T>& size) : 
			left(pos.x), top(pos.y), nearValue(pos.z), width(size.x), height(size.y), depth(size.z) { }

		inline const vector3<T>& origin() const 
			{ return *((vector3<T>*)(c)); }

		inline const vector3<T>& size() const 
			{ return *((vector3<T>*)(c+3)); }

		inline T right() const 
			{ return left + width; }

		inline T bottom() const 
			{ return top + height; }

		inline T farValue() const 
			{ return nearValue + depth; }

		inline T volume() const
			{ return width * height * depth; }

		inline void setOrigin(const vector3<T>& p)
		{ 
			left = p.x;
			top = p.y;
			nearValue = p.z;
		}

		inline void setSize(const vector3<T>& s)
		{ 
			width = s.x;
			height = s.y;
			depth = s.z;
		}

		inline Parallelepiped operator * (T v) const
			{ return Parallelepiped(left * v, top * v, nearValue * v, width * v, height * v, depth * v); }

		inline Parallelepiped operator + (const Parallelepiped& r) const
			{ return Parallelepiped(left + r.left, top + r.top, nearValue + r.nearValue, width + r.width, height + r.height, depth + r.depth); }

		inline bool containPoint(const vector3<T>& p) const
			{ return (p.x >= left) && (p.y >= top) && (p.z >= nearValue) && (p.x < right()) && (p.y < bottom()) && (p.z <= farValue()); }

	};
}