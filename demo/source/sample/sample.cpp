//
//  sample.cpp
//  osx
//
//  Created by Sergey Reznik on 30.05.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#include <et/primitives/primitives.h>
#include <et/camera/camera.h>
#include "sample.h"

using namespace demo;
using namespace et;

void Sample::prepare(et::RenderContext* rc)
{
	_program = rc->programFactory().loadProgram("data/shaders/pgrid.program");
	
	_camera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 1024.0f);
	_camera.lookAt(vec3(10.0f));

	vec2i gridSize(100);
	VertexDeclaration decl(true, Usage_Position, Type_Vec2);
	VertexArray va(decl, gridSize.square());
	va.retain();
	
	RawDataAcessor<vec2> pos = va.chunk(Usage_Position).accessData<vec2>(0);
	size_t k = 0;
	for (int y = 0; y < gridSize.y; ++y)
	{
		for (int x = 0; x < gridSize.x; ++x)
		{
			pos[k++] = vec2(static_cast<float>(x) / static_cast<float>(gridSize.x - 1),
				static_cast<float>(y) / static_cast<float>(gridSize.y - 1));
		}
	}

	IndexArray ia(IndexArrayFormat_16bit, primitives::indexCountForRegularMesh(gridSize,
		PrimitiveType_TriangleStrips), PrimitiveType_TriangleStrips);
	ia.retain();
	
	primitives::buildTriangleStripIndexes(ia, gridSize, 0, 0);

	_vao = rc->vertexBufferFactory().createVertexArrayObject("grid", VertexArray::Pointer(&va),
		BufferDrawType_Static, IndexArray::Pointer(&ia), BufferDrawType_Static);
}

void Sample::render(et::RenderContext* rc)
{
	rc->renderState().bindProgram(_program);
	rc->renderState().bindVertexArray(_vao);
	rc->renderState().setWireframeRendering(true);
	rc->renderer()->drawAllElements(_vao->indexBuffer());
	rc->renderState().setWireframeRendering(false);
}
