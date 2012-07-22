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
	struct Rect
	{
		union
		{
			struct
			{
				T left;
				T top;
				T width;
				T height;
			};

			T c[4];
		};

		Rect() : left(0), top(0), width(0), height(0) 
			{ }

		Rect(T x, T y, T w, T h) : left(x), top(y), width(w), height(h) 
			{ }

		Rect(const vector2<T>& pos, const vector2<T>& size) : 
			left(pos.x), top(pos.y), width(size.x), height(size.y) { }

		T& operator [](int i) 
			{ return c[i]; }

		const T& operator [](int i) const
			{ return c[i]; }

		const vector2<T>& origin() const 
			{ return *(reinterpret_cast<const vector2<T>*>(c)); }

		const vector2<T>& size() const 
			{ return *(reinterpret_cast<const vector2<T>*>(c+2)); }

		T right() const 
			{ return left + width; }

		T bottom() const 
			{ return top + height; }

		T square() const
			{ return width * height; }

		void setOrigin(const vector2<T>& p)
			{ left = p.x; top = p.y; }

		void setSize(const vector2<T>& s)
			{ width = s.x; height = s.y; }

		Rect operator * (T v) const
			{ return Rect(left * v, top * v, width * v, height * v); }

		Rect operator + (const Rect& r) const
			{ return Rect(left + r.left, top + r.top, width + r.width, height + r.height); }

		bool containsPoint(const vector2<T>& p) const
			{ return (p.x >= left) && (p.y >= top) && (p.x < right()) && (p.y < bottom()); }

		vector2<T> leftTop() const 
			{ return vector2<T>(left, top); }

		vector2<T> leftBottom() const 
			{ return vector2<T>(left, bottom()); }

		vector2<T> rightTop() const 
			{ return vector2<T>(right(), top); }

		vector2<T> rightBottom() const 
			{ return vector2<T>(right(), bottom()); }

		vector2<T> center() const 
			{ return vector2<T>(left + width / static_cast<T>(2), top + height / static_cast<T>(2)); }

		bool intersects(const Rect& r) const
		{
			if (r.left >= left + width) return false;
			if (r.top >= top + height) return false;
			if (r.left + r.width < left) return false;	
			if (r.top + r.height < top) return false;
			return true;
		}

		T aspect() const
			{ return width / height; }
	};

}