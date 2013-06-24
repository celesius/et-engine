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

vec4 getPosition(const vec2& p, const mat4& invProj, const mat4& proj);

void Sample::prepare(et::RenderContext* rc)
{
	_texture = rc->textureFactory().loadTexture("data/textures/clouds.png", _cache);

	_cameraAngles.setValue(vec2(0.0f, 0.0f));
	_cameraPosition.setValue(vec3(50.0f));

	ET_CONNECT_EVENT(_cameraAngles.updated, Sample::updateCamera)
	ET_CONNECT_EVENT(_cameraPosition.updated, Sample::updateCamera)

	_cameraAngles.run();
	_cameraPosition.run();

	loadPrograms(rc);
	initCamera(rc);
	createGeometry(rc);

	updateCamera();
}

void Sample::loadPrograms(et::RenderContext* rc)
{
	_program = rc->programFactory().loadProgram("data/shaders/pgrid.program");
	_program->setUniform("cloudsTexture", 0);
}

void Sample::initCamera(et::RenderContext* rc)
{
	_camera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 1000.0f);
	_camera.lookAt(vec3(50.0f, 15.0f, 50.0f));
}

void Sample::createGeometry(et::RenderContext* rc)
{
	vec2i gridSize(225);
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
			pos[k++] = vec2(-1.0f + 2.0f * static_cast<float>(x) / static_cast<float>(gridSize.x - 1),
				-1.0f + 2.0f * static_cast<float>(y) / static_cast<float>(gridSize.y - 1));
		}
	}

	primitives::buildTriangleStripIndexes(ia, gridSize, 0, 0);

	_vao = rc->vertexBufferFactory().createVertexArrayObject("grid", VertexArray::Pointer(&va),
		BufferDrawType_Static, IndexArray::Pointer(&ia), BufferDrawType_Static);
}

void Sample::render(et::RenderContext* rc)
{
	rc->renderState().bindProgram(_program);
	_program->setCameraProperties(_camera);
	_program->setUniform("mInverseMVPMatrix", _projectorCamera);

	rc->renderState().bindTexture(0, _texture);
	rc->renderState().bindVertexArray(_vao);
//	rc->renderState().setWireframeRendering(true);
	rc->renderer()->drawAllElements(_vao->indexBuffer());
//	rc->renderState().setWireframeRendering(false);
}

void Sample::dragCamera(const et::vec2& v)
{
	_cameraAngles.addVelocity(0.1f * v);
}

void Sample::updateCamera()
{
	vec3 pos = _cameraPosition.value();
	vec3 dir = fromSpherical(_cameraAngles.value().y, _cameraAngles.value().x);

	_camera.lookAt(pos, pos + dir);
	updateProjectorMatrix();
}

void Sample::panCamera(const et::vec2& v)
{
	vec3 forward = _camera.direction() * vec3(1.0f, 0.0f, 1.0f);
	vec3 side = _camera.side() * vec3(1.0f, 0.0f, 1.0f);
	_cameraPosition.addVelocity(v.x * side - v.y * forward);
}

void Sample::stopCamera()
{
	_cameraPosition.setVelocity(vec3(0.0f));
	_cameraAngles.setVelocity(vec2(0.0f));
}

void Sample::zoom(float v)
{
	_cameraPosition.addVelocity(0.25f * _camera.position().length() * _camera.direction() * (v - 1.0f));
}

void Sample::updateProjectorMatrix()
{
	_projectorCamera = _camera.inverseModelViewProjectionMatrix();
}