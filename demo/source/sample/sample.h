//
//  sample.h
//  osx
//
//  Created by Sergey Reznik on 30.05.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#pragma once

#include <et/rendering/rendercontext.h>
#include <et/collision/collision.h>

namespace demo
{
	class Sample
	{
	public:
		void prepare(et::RenderContext*);
		void render(et::RenderContext*);

	private:
		void loadPrograms(et::RenderContext*);
		void initCamera(et::RenderContext*);
		void createGeometry(et::RenderContext*);

	private:
		typedef et::StaticDataStorage<et::vec3, et::AABBCorner_max> FrustumVec3Points;
		typedef et::StaticDataStorage<et::vec4, et::AABBCorner_max> FrustumVec4Points;
		typedef et::StaticDataStorage<et::vec4, 4> PlanePoints;

	private:
		et::VertexArrayObject _vao;
		et::Camera _camera;
		et::Program _program;

		PlanePoints _corners;
	};
}