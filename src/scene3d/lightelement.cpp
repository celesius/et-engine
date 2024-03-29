/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/scene3d/lightelement.h>

using namespace et;
using namespace et::s3d;

LightElement::LightElement(const std::string& name, Element* parent) : Element(name, parent)
{

}

LightElement* LightElement::duplicate()
{
	LightElement* result = new LightElement(name(), parent());

	duplicateBasePropertiesToObject(result);
	duplicateChildrenToObject(result);

	result->setModelViewMatrix(modelViewMatrix());
	result->setProjectionMatrix(projectionMatrix());
	
	if (upVectorLocked())
		result->lockUpVector(lockedUpVector());

	return result;
}

void LightElement::serialize(std::ostream& stream, SceneVersion version)
{
	serializeMatrix(stream, modelViewMatrix());
	serializeMatrix(stream, projectionMatrix());
	serializeInt(stream, upVectorLocked());
	serializeVector(stream, lockedUpVector());

	serializeGeneralParameters(stream, version);
	serializeChildren(stream, version);
}

void LightElement::deserialize(std::istream& stream, ElementFactory* factory, SceneVersion version)
{
//	mat4 mv =
	deserializeMatrix(stream);
//	mat4 proj =
	deserializeMatrix(stream);
//	bool upLocked =
    deserializeInt(stream);
//	vec3 locked =
	deserializeVector<vec3>(stream);

	deserializeGeneralParameters(stream, version);
	deserializeChildren(stream, factory, version);
}

