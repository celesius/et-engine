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