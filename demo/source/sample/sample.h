//
//  sample.h
//  osx
//
//  Created by Sergey Reznik on 30.05.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#pragma once

#include <et/rendering/rendercontext.h>
#include <et/timers/inertialvalue.h>

namespace demo
{
	class Sample : public et::EventReceiver
	{
	public:
		void prepare(et::RenderContext*);
		void render(et::RenderContext*);

		void stopCamera();
		void dragCamera(const et::vec2&);
		void panCamera(const et::vec2&);
		void zoom(float);

	private:
		void loadPrograms(et::RenderContext*);
		void initCamera(et::RenderContext*);
		void createGeometry(et::RenderContext*);
		
		void updateCamera();
		void updateProjectorMatrix();

	private:
		typedef et::StaticDataStorage<et::vec4, 4> PlanePoints;

	private:
		et::ObjectsCache _cache;
		
		et::VertexArrayObject _vao;
		et::Camera _camera;
		et::Program _program;
		et::Texture _texture;

		et::InertialValue<et::vec2> _cameraAngles;
		et::InertialValue<et::vec3> _cameraPosition;

		et::mat4 _projectorCamera;
	};
}