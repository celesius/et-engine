#include <et/scene3d/cameraelement.h>

using namespace et;
using namespace et::s3d;

CameraElement::CameraElement(const std::string& name, Element* parent) : Element(name, parent)
{

}

CameraElement* CameraElement::duplicate()
{
	CameraElement* result = new CameraElement(name(), parent());
	result->_camera = _camera;
	return result;
}

void CameraElement::serialize(std::ostream& stream)
{
	serializeMatrix(stream, _camera.modelViewMatrix());
	serializeMatrix(stream, _camera.projectionMatrix());
	serializeInt(stream, _camera.upVectorLocked());
	serializeVector(stream, _camera.lockedUpVector());

	serializeGeneralParameters(stream);
	serializeChildren(stream);
}

void CameraElement::deserialize(std::istream& stream, ElementFactory* factory)
{
	mat4 mv = deserializeMatrix(stream);
	mat4 proj = deserializeMatrix(stream);
	bool upLocked = deserializeInt(stream) != 0;
	vec3 locked = deserializeVector<vec3>(stream);

	deserializeGeneralParameters(stream);
	deserializeChildren(stream, factory);
}

