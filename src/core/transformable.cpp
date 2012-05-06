/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <assert.h>
#include <iostream>
#include <et/core/transformable.h>

using namespace et;

ComponentTransformable::ComponentTransformable() : _cachedTransform(IDENTITY_MATRIX), 
	_translation(0.0f), _scale(1.0f), _orientation(), _transformValid(false)
{
}

mat4 ComponentTransformable::transform()
{
	if (!_transformValid)
		buildTransform();

	return _cachedTransform;
}

void ComponentTransformable::buildTransform()
{
	_cachedTransform = scaleMatrix(_scale) * _orientation.toMatrix();
	_cachedTransform[3] = vec4(_translation, 1.0f);
	_transformValid = true;
}

void ComponentTransformable::invalidateTransform()
{
	_transformValid = false;
}

void ComponentTransformable::setTranslation(const vec3& t)
{
	_translation = t;
	invalidateTransform();
}

void ComponentTransformable::applyTranslation(const vec3& t)
{
	_translation += t;
	invalidateTransform();
}

void ComponentTransformable::setScale(const vec3& s)
{
	_scale = s;
	invalidateTransform();
}

void ComponentTransformable::applyScale(const vec3& s)
{
	_scale *= s;
	invalidateTransform();
}

void ComponentTransformable::setOrientation(const quaternion& q)
{
	_orientation = q;
	invalidateTransform();
}

void ComponentTransformable::applyOrientation(const quaternion& q)
{
	_orientation *= q;
	invalidateTransform();
}

#if (ET_DEBUG)

void ComponentTransformable::setTransform(mat4 originalMatrix)
{
	mat4 m = originalMatrix;
	mat3 rotationMatrix = m.mat3();
	_scale = removeMatrixScaleRowMajor(rotationMatrix);
	_orientation = matrixToQuaternion(rotationMatrix);
	_translation = m[3].xyz();
	buildTransform();
	float deviation = 0.0f;
	for (size_t v = 0; v < 4; ++v)
	{
		for (size_t u = 0; u < 4; ++u)
			deviation += sqr(originalMatrix[v][u] - _cachedTransform[v][u]);
	}
	if (deviation > 0.01f)
		std::cout << "Failed to convert matrix to quternion, deviation = " << deviation << std::endl;
}

#else

void ComponentTransformable::setTransform(mat4 m)
{
	mat3 rotationMatrix = m.mat3();
	_scale = removeMatrixScaleRowMajor(rotationMatrix);
	_orientation = matrixToQuaternion(rotationMatrix);
	_translation = m[3].xyz();
	invalidateTransform();
}

#endif
