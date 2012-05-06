/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	class MatrixTransformable
	{ 
	public:
		MatrixTransformable() : _transform(IDENTITY_MATRIX)
			{ }

		MatrixTransformable(const mat4& t) : _transform(t)
			{ }

		virtual ~MatrixTransformable() { }

		virtual mat4 transform() 
			{ return _transform; }

		virtual const mat4& transform() const
			{ return _transform; }

		virtual void setTransform(const mat4& transform)
			{ _transform = transform; }

		virtual void applyTransform(const mat4& transform)
			{ _transform *= transform; }

	private:
		mat4 _transform;
	};

	class ComponentTransformable
	{
	public:
		ComponentTransformable();
		virtual ~ComponentTransformable() { }

		virtual mat4 transform();
		virtual void setTransform(mat4 m);

		virtual void setTranslation(const vec3& t);
		virtual void applyTranslation(const vec3& t);
		
		virtual void setScale(const vec3& s);
		virtual void applyScale(const vec3& s);
		
		virtual void setOrientation(const quaternion& q);
		virtual void applyOrientation(const quaternion& q);

		virtual void invalidateTransform();
		
		bool transformValid() const
			{ return _transformValid; }
		const vec3& translation() const
			{ return _translation; }
		const vec3& scale() const
			{ return _scale; }
		const quaternion& orientation() const
			{ return _orientation; }

	private:
		void buildTransform();

	private:
		mat4 _cachedTransform;
		vec3 _translation;
		vec3 _scale;
		quaternion _orientation;
		bool _transformValid;
	};
}