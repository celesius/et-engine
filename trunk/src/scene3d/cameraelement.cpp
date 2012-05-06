/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/scene3d/cameraelement.h>

using namespace et;
using namespace et::s3d;

CameraElement::CameraElement(const std::string& name, Element* parent) : Element(name, parent)
{

}

CameraElement* CameraElement::duplicate()
{
	CameraElement* result = new CameraElement(name(), parent());
	result->setModelViewMatrix(modelViewMatrix());
	result->setProjectionMatrix(projectionMatrix());
	if (upVectorLocked())
		result->lockUpVector(lockedUpVector());
	return result;
}

void CameraElement::serialize(std::ostream& stream)
{
	serializeMatrix(stream, modelViewMatrix());
	serializeMatrix(stream, projectionMatrix());
	serializeInt(stream, upVectorLocked());
	serializeVector(stream, lockedUpVector());

	serializeGeneralParameters(stream);
	serializeChildren(stream);
}

void CameraElement::deserialize(std::istream& stream, ElementFactory* factory)
{
	mat4 mv = deserializeMatrix(stream);
	mat4 proj = deserializeMatrix(stream);
    //	bool upLocked = deserializeInt(stream) != 0;
    deserializeInt(stream);
	vec3 locked = deserializeVector<vec3>(stream);

	deserializeGeneralParameters(stream);
	deserializeChildren(stream, factory);
}

