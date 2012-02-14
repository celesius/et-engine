#pragma once

#include <et/camera/camera.h>
#include <et/scene3d/element.h>

namespace et
{
	namespace s3d
	{
		class CameraElement : public Element
		{
		public:
			typedef IntrusivePtr<CameraElement> Pointer;

		public:
			CameraElement(const std::string& name, Element* parent);

			Camera& camera()
				{ return _camera; }

			const Camera& camera() const
				{ return _camera; }

			ElementType type() const 
				{ return ElementType_Camera; }

			CameraElement* duplicate();

		private:
			Camera _camera;
		};
	}
}