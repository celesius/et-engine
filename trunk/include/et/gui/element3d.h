#pragma once

#include <et/gui/guibase.h>

namespace et
{
	namespace gui
	{
		
		class Element3D : public Element
		{
		public:
			Element3D(const Camera& camera, Element* parent);

			virtual ElementClass elementClass() const 
				{ return ElementClass_3d; };

			void setTransform(const mat4& transform);
			void applyTransform(const mat4& transform);

			const mat4& transform() const 
				{ return _transform; }

			const mat4& inverseFinalTransform();

			const vec2& position() const 
				{ return _center; }

			BaseAnimator* setPosition(const vec2&, float) 
				{ return 0; }

		protected:
			mat4 finalTransform();
			void buildFinalTransform();

			const Camera& camera() const 
				{ return _camera; }

		private:
			const Camera& _camera;

			vec2 _center;
			mat4 _transform;
			mat4 _finalTransform;
			mat4 _inverseFinalTransform;

			bool _finalTransformValid;
			bool _inverseFinalTransformValid;
		};

	}
}