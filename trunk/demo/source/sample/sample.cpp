//
//  sample.cpp
//  osx
//
//  Created by Sergey Reznik on 30.05.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#include <et/core/tools.h>
#include <et/primitives/primitives.h>
#include <et/camera/camera.h>
#include "sample.h"

using namespace demo;
using namespace et;

void Sample::prepare(et::RenderContext* rc)
{
//	loadPrograms(rc);
	initCamera(rc);
	createGeometry(rc);

	mat4 mvp = _camera.modelViewProjectionMatrix();
	mat4 imvp = _camera.inverseModelViewProjectionMatrix();

	FrustumVec3Points corners;
	corners[AABBCorner_LeftUpFar]     = imvp * vec3(-1.0f, -1.0f,  1.0f);
	corners[AABBCorner_RightUpFar]    = imvp * vec3( 1.0f, -1.0f,  1.0f);
	corners[AABBCorner_LeftDownFar]   = imvp * vec3(-1.0f,  1.0f,  1.0f);
	corners[AABBCorner_RightDownFar]  = imvp * vec3( 1.0f,  1.0f,  1.0f);
	corners[AABBCorner_LeftUpNear]    = imvp * vec3(-1.0f, -1.0f, -1.0f);
	corners[AABBCorner_RightUpNear]   = imvp * vec3( 1.0f, -1.0f, -1.0f);
	corners[AABBCorner_LeftDownNear]  = imvp * vec3(-1.0f,  1.0f, -1.0f);
	corners[AABBCorner_RightDownNear] = imvp * vec3( 1.0f,  1.0f, -1.0f);

	vec3 pNormal = unitY;
	plane pUpper(pNormal, 1.0f);
	plane pLower(pNormal, -1.0f);

	FrustumVec4Points projected;
	for (size_t i = AABBCorner_First; i < AABBCorner_LeftUpNear; ++i)
	{
		vec3 origin = corners[i + 4];
		vec3 end = corners[i];
		vec3 direction = normalize(end - origin);

		vec3 ipUpper;
		vec3 ipLower;
		intersect::rayPlane(ray3d(origin, direction), pUpper, &ipUpper);
		intersect::rayPlane(ray3d(origin, direction), pLower, &ipLower);

		vec3 ipUpperProj = ipUpper - pNormal * dot(pNormal, ipUpper);
		vec3 ipLowerProj = ipLower - pNormal * dot(pNormal, ipLower);
		projected[i] = mvp * vec4(ipLowerProj, 1.0f);
		projected[i+4] = mvp * vec4(ipUpperProj, 1.0f);

		std::cout << "{" << std::endl << "\tR: " << origin << " -> " << end << std::endl <<
			"\tD: " << direction << std::endl <<
			"\tU: " << ipUpper << std::endl <<
			"\tL: " << ipLower << std::endl <<
			"\tPU: " << projected[i+4] << std::endl <<
			"\tPL: " << projected[i] << std::endl << "}" << std::endl;
	}
}

void Sample::loadPrograms(et::RenderContext* rc)
{
	_program = rc->programFactory().loadProgram("data/shaders/pgrid.program");
}

void Sample::initCamera(et::RenderContext* rc)
{
	_camera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 1000.0f);
	_camera.lookAt(vec3(50.0f));
}

void Sample::createGeometry(et::RenderContext* rc)
{
	vec2i gridSize(100);
	VertexArray va(VertexDeclaration(false, Usage_Position, Type_Vec2), gridSize.square());
	va.retain();

	IndexArray ia(IndexArrayFormat_16bit, primitives::indexCountForRegularMesh(gridSize,
		PrimitiveType_TriangleStrips), PrimitiveType_TriangleStrips);
	ia.retain();

	RawDataAcessor<vec2> pos = va.chunk(Usage_Position).accessData<vec2>(0);
	for (int y = 0, k = 0; y < gridSize.y; ++y)
	{
		for (int x = 0; x < gridSize.x; ++x)
		{
			pos[k++] = vec2(static_cast<float>(x) / static_cast<float>(gridSize.x - 1),
				static_cast<float>(y) / static_cast<float>(gridSize.y - 1));
		}
	}

	primitives::buildTriangleStripIndexes(ia, gridSize, 0, 0);

	_vao = rc->vertexBufferFactory().createVertexArrayObject("grid", VertexArray::Pointer(&va),
		BufferDrawType_Static, IndexArray::Pointer(&ia), BufferDrawType_Static);
}

void Sample::render(et::RenderContext* rc)
{
	if (_program.invalid())
		loadPrograms(rc);

	rc->renderState().bindProgram(_program);
	rc->renderState().bindVertexArray(_vao);
	rc->renderState().setWireframeRendering(true);
	rc->renderer()->drawAllElements(_vao->indexBuffer());
	rc->renderState().setWireframeRendering(false);
}
