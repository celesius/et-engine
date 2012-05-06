/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/guirenderer.h>
#include <et/gui/element3d.h>

using namespace et;
using namespace et::gui;

Element3D::Element3D(const Camera& camera, Element* parent) : Element(parent), 
	_camera(camera), _transform(IDENTITY_MATRIX), _finalTransform(IDENTITY_MATRIX), 
	_inverseFinalTransform(IDENTITY_MATRIX), _finalTransformValid(true), _inverseFinalTransformValid(true)
{

}

mat4 Element3D::finalTransform()
{
	if (!_finalTransformValid)
		buildFinalTransform();

	return _finalTransform;
}

const mat4& Element3D::inverseFinalTransform()
{
	if (!_inverseFinalTransformValid)
	{
		_inverseFinalTransform = finalTransform().inverse();
		_inverseFinalTransformValid = true;
	}

	return _inverseFinalTransform;
}

void Element3D::buildFinalTransform()
{
	_finalTransform = parent() ? _transform * parent()->finalTransform() : _transform;
	_finalTransformValid = false;
	_inverseFinalTransformValid = false;
}

void Element3D::setTransform(const mat4& t)
{
	_transform = t;
	_finalTransformValid = false;
	_inverseFinalTransformValid = false;
}

void Element3D::applyTransform(const mat4& t)
{
	_transform *= t;
	_finalTransformValid = false;
	_inverseFinalTransformValid = false;
}
