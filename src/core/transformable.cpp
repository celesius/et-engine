/*
* This file is part of `et engine`
* Copyright 2009-2013 by Sergey Reznik
* Please, do not modify content without approval.
*
*/

#include <assert.h>
#include <et/core/transformable.h>

using namespace et;

ComponentTransformable::ComponentTransformable() : _cachedTransform(identityMatrix), 
	_translation(0.0f), _scale(1.0f), _orientation(), _flags(0)
{
}

mat4 ComponentTransformable::transform()
{
	if (!transformValid())
		buildTransform();

	return _cachedTransform;
}

void ComponentTransformable::buildTransform()
{
	if (!shouldDecompose())
	{
		_cachedTransform = _orientation.toMatrix() * scaleMatrix(_scale);
		_cachedTransform[3] = vec4(_translation, 1.0f);
	}

	_flags |= Flag_Valid;
}

void ComponentTransformable::invalidateTransform()
{
	_flags &= ~Flag_Valid;
}

void ComponentTransformable::setTranslation(const vec3& t)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_translation = t;
	invalidateTransform();
}

void ComponentTransformable::applyTranslation(const vec3& t)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_translation += t;
	invalidateTransform();
}

void ComponentTransformable::setScale(const vec3& s)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_scale = s;
	invalidateTransform();
}

void ComponentTransformable::applyScale(const vec3& s)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_scale *= s;
	invalidateTransform();
}

void ComponentTransformable::setOrientation(const quaternion& q)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_orientation = q;
	invalidateTransform();
}

void ComponentTransformable::applyOrientation(const quaternion& q)
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	_orientation *= q;
	invalidateTransform();
}

const mat4& ComponentTransformable::cachedTransform() const
{
	assert(transformValid());
	return _cachedTransform; 
}

void ComponentTransformable::setTransform(const mat4& originalMatrix)
{
	mat3 rotationMatrix = originalMatrix.mat3();
	
	_scale = removeMatrixScaleRowMajor(rotationMatrix);
	_orientation = matrixToQuaternion(rotationMatrix);
	_translation = originalMatrix[3].xyz();
	_flags &= ~Flag_ShouldDecompose;

	invalidateTransform();

#if (ET_DEBUG)
	buildTransform();

	float deviation = 0.0f;
	for (size_t v = 0; v < 4; ++v)
		for (size_t u = 0; u < 4; ++u)
			deviation += sqr(originalMatrix[v][u] - _cachedTransform[v][u]);

	if (deviation > 0.01f)
	{
		log::warning("Failed to decompose matrix\n{\n\tscale: (%f %f %f)\n\torientation: (%f %f %f %f)\n\t"
			"translation: (%f %f %f)\n\tdeviation: %f\n}", _scale.x, _scale.y, _scale.z, _orientation.scalar,
			_orientation.vector.x, _orientation.vector.y, _orientation.vector.z,
			_translation.x, _translation.y, _translation.z, deviation);
	}
#endif
}

void ComponentTransformable::setTransformDirectly(const mat4& m)
{
	_cachedTransform = m;
	_flags |= Flag_ShouldDecompose;
	_flags &= ~Flag_Valid;
}

const vec3& ComponentTransformable::translation() const
{ 
	return shouldDecompose() ? _cachedTransform[3].xyz() : _translation;
}

const vec3& ComponentTransformable::scale()
{ 
	if (shouldDecompose())
		setTransform(_cachedTransform);

	return _scale; 
}

const quaternion& ComponentTransformable::orientation()
{
	if (shouldDecompose())
		setTransform(_cachedTransform);

	return _orientation; 
}