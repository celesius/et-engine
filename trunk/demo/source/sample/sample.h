//
//  sample.h
//  osx
//
//  Created by Sergey Reznik on 30.05.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#pragma once

#include <et/rendering/rendercontext.h>

namespace demo
{
	class Sample
	{
	public:
		void prepare(et::RenderContext*);

		void render(et::RenderContext*);

	private:
		et::VertexArrayObject _vao;
		et::Camera _camera;
		et::Program _program;
	};
}