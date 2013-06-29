//
// sample.cpp
// osx
//
// Created by Sergey Reznik on 30.05.13.
// Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#include <et/core/tools.h>
#include <et/primitives/primitives.h>
#include <et/camera/camera.h>
#include "sample.h"

using namespace demo;
using namespace et;

size_t frustumLines[] = {0,1, 0,2, 2,3, 1,3, 0,4, 2,6, 3,7, 1,5, 4,6, 4,5, 5,7, 6,7};
size_t numFrustumLines = sizeof(frustumLines) / (2 * sizeof(decltype(*frustumLines)));

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
	createFrustumGeometry(rc);

	updateCamera();
}

void Sample::loadPrograms(et::RenderContext* rc)
{
	_program = rc->programFactory().loadProgram("data/shaders/pgrid.program");
	_program->setUniform("cloudsTexture", 0);

	_frustumProgram = rc->programFactory().loadProgram("data/shaders/lines.program");
}

void Sample::initCamera(et::RenderContext* rc)
{
	_camera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 500.0f);

	_observingCamera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 2000.f);
	_observingCamera.lookAt(vec3(-200.0f, 200.0f, -200.0f), vec3(100.0f, 0.0f, 100.0f));
}

void Sample::createGeometry(et::RenderContext* rc)
{
	vec2i gridSize(220);
	VertexArray va(VertexDeclaration(false, Usage_Position, Type_Vec2), gridSize.square());
	va.retain();

	IndexArray ia(IndexArrayFormat_16bit, primitives::indexCountForRegularMesh(gridSize,
		PrimitiveType_TriangleStrips), PrimitiveType_TriangleStrips);
	ia.retain();

	RawDataAcessor<vec2> pos = va.chunk(Usage_Position).accessData<vec2>(0);
	size_t k = 0;
	float dx = 1.0f / static_cast<float>(gridSize.x - 1);
	float dy = 1.0f / static_cast<float>(gridSize.y - 1);
	for (int y = gridSize.y - 1; y >= 0; --y)
	{
		for (int x = 0; x < gridSize.x; ++x)
			pos[k++] = vec2(-1.0f) + 2.0f * vec2(static_cast<float>(x) * dx, static_cast<float>(y) * dy);
	}

	primitives::buildTriangleStripIndexes(ia, gridSize, 0, 0);

	_vao = rc->vertexBufferFactory().createVertexArrayObject("grid", VertexArray::Pointer(&va),
		BufferDrawType_Static, IndexArray::Pointer(&ia), BufferDrawType_Static);
}

void Sample::render(et::RenderContext* rc)
{
	bool observing = false;
	Camera& cam = observing ? _observingCamera : _camera;

	rc->renderState().setWireframeRendering(true);

	rc->renderState().setDepthTest(true);
	rc->renderState().setDepthMask(true);
	rc->renderState().setDepthFunc(DepthFunc_Less);
	rc->renderState().setCulling(CullState_Back);

	if (_shouldRenderGrid)
	{
		rc->renderState().bindProgram(_program);
		_program->setCameraProperties(cam);
		_program->setUniform("mInverseMVPMatrix", _projectorMatrix);
		rc->renderState().bindTexture(0, _texture);
		rc->renderState().bindVertexArray(_vao);
		rc->renderer()->drawAllElements(_vao->indexBuffer());
	}

	rc->renderState().bindProgram(_frustumProgram);
	_frustumProgram->setCameraProperties(cam);
	_frustumProgram->setUniform("linesColor", _shouldRenderGrid ? vec4(1.0f) : vec4(1.0f, 0.5f, 0.25f, 1.0f));
	rc->renderState().bindVertexArray(_frustumGeometry);
	rc->renderer()->drawAllElements(_frustumGeometry->indexBuffer());

	rc->renderState().setWireframeRendering(false);
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
	_projectorCamera = Camera(_camera);
	StaticDataStorage<vec3, 24> projPoints;

	size_t numPoints = 0;
	updateFrustumGeometry(false);

	float amplitude = 14.0f;
	plane mainPlane(0.0f, 1.0f, 0.0f, 0.0f);
	plane upperBound(0.0f, 1.0f, 0.0f, amplitude);
	plane lowerBound(0.0f, 1.0f, 0.0f, -amplitude);

	for (size_t i = 0; i < numFrustumLines; ++i)
	{
		size_t src = frustumLines[i*2];
		size_t dst = frustumLines[i*2 + 1];
		segment3d seg(_frustumLines[src], _frustumLines[dst]);
		if (intersect::segmentPlane(seg, upperBound, &projPoints[numPoints])) ++numPoints;
		if (intersect::segmentPlane(seg, lowerBound, &projPoints[numPoints])) ++numPoints;
	}

	for (size_t i = 0; i < _frustumLines.size(); ++i)
	{
		if (upperBound.distanceToPoint(_frustumLines[i]) * lowerBound.distanceToPoint(_frustumLines[i]) < 0.0f)
			projPoints[numPoints++] = _frustumLines[i];
	}

	_shouldRenderGrid = numPoints > 0;
	
	float distanceToPlane = mainPlane.distanceToPoint(_projectorCamera.position());
	if (distanceToPlane < amplitude)
	{
		bool belowSurface = distanceToPlane < 0.0f;
		_projectorCamera.setPosition(_projectorCamera.position() +
			mainPlane.normal() * (amplitude - (belowSurface ? 2.0f : 1.0f) * distanceToPlane));
	}

	vec3 intersectionPoint;
	ray3d cameraRay(_projectorCamera.position(), -_projectorCamera.direction());
	if (!intersect::rayPlane(cameraRay, mainPlane, &intersectionPoint))
	{
		cameraRay.direction -= 2.0f * mainPlane.normal() * dot(cameraRay.direction, mainPlane.normal());
		intersect::rayPlane(cameraRay, mainPlane, &intersectionPoint);
	}

	float af = std::abs(dot(mainPlane.normal(), cameraRay.direction));
	vec3 farPoint = _projectorCamera.unproject(vec3(0.0f, 0.0f, 1.0f));
	farPoint = mainPlane.projectionOfPoint(farPoint);
	intersectionPoint = mix(farPoint, intersectionPoint, af);
	_projectorCamera.setDirection(normalize(intersectionPoint - _projectorCamera.position()));

	for (size_t i = 0; i < numPoints; ++i)
		projPoints[i] = _projectorCamera.project(mainPlane.projectionOfPoint(projPoints[i]));

	float minX = 0.0f;
	float maxX = 1.0f;
	float minY = 0.0f;
	float maxY = 1.0f;
/* *
	minX = projPoints[0].x;
	maxX = projPoints[0].x;
	minY = projPoints[0].y;
	maxY = projPoints[0].y;
	for (size_t i = 1; i < numPoints; i++)
	{
		maxX = etMax(maxX, projPoints[i].x);
		minX = etMin(minX, projPoints[i].x);
		maxY = etMax(maxY, projPoints[i].y);
		minY = etMin(minY, projPoints[i].y);
	}

	log::info("%zu (%f -> %f), (%f -> %f)", numPoints, minX, maxX, minY, maxY);
// */
	
	mat4 mRange;
	mRange[0][0] = maxX - minX;
	mRange[1][1] = maxY - minY;
	mRange[2][2] = 1.0f;
	mRange[3][0] = minX;
	mRange[3][1] = minY;
	mRange[3][3] = 1.0f;

	_projectorMatrix = mRange * _projectorCamera.inverseModelViewProjectionMatrix();

	updateFrustumGeometry(true);
}

void Sample::createFrustumGeometry(et::RenderContext* rc)
{
	VertexDeclaration decl(false, Usage_Position, Type_Vec3);
	decl.push_back(Usage_Color, Type_Vec4);
	_frustumLinesData = VertexArray::Pointer(new VertexArray(decl, 2 * _frustumLines.size()));
	
	IndexArray ai(IndexArrayFormat_8bit, 4 * numFrustumLines, PrimitiveType_Lines);
	size_t k = 0;
	for (size_t i = 0; i < numFrustumLines; ++i)
	{
		ai.setIndex(frustumLines[2*i+0], k++);
		ai.setIndex(frustumLines[2*i+1], k++);
	}

	for (size_t i = 0; i < numFrustumLines; ++i)
	{
		ai.setIndex(_frustumLines.size() + frustumLines[2*i+0], k++);
		ai.setIndex(_frustumLines.size() + frustumLines[2*i+1], k++);
	}
	
	ai.retain();
	
	_frustumGeometry = rc->vertexBufferFactory().createVertexArrayObject("frustum", _frustumLinesData,
		BufferDrawType_Stream, IndexArray::Pointer(&ai), BufferDrawType_Static);
}

void Sample::updateFrustumGeometry(bool projector)
{
	_frustumLines[0] = _projectorCamera.unproject(vec3(-1.0f, -1.0f, -1.0f));
	_frustumLines[1] = _projectorCamera.unproject(vec3(+1.0f, -1.0f, -1.0f));
	_frustumLines[2] = _projectorCamera.unproject(vec3(-1.0f, +1.0f, -1.0f));
	_frustumLines[3] = _projectorCamera.unproject(vec3(+1.0f, +1.0f, -1.0f));
	_frustumLines[4] = _projectorCamera.unproject(vec3(-1.0f, -1.0f, +1.0f));
	_frustumLines[5] = _projectorCamera.unproject(vec3(+1.0f, -1.0f, +1.0f));
	_frustumLines[6] = _projectorCamera.unproject(vec3(-1.0f, +1.0f, +1.0f));
	_frustumLines[7] = _projectorCamera.unproject(vec3(+1.0f, +1.0f, +1.0f));

	size_t off = projector ? _frustumLines.size() : 0;
	vec4 aColor = projector ? vec4(1.0f, 1.0f, 0.0f, 1.0f) : vec4(1.0f);
	
	RawDataAcessor<vec3> pos = _frustumLinesData->chunk(Usage_Position).accessData<vec3>(0);
	RawDataAcessor<vec4> clr = _frustumLinesData->chunk(Usage_Color).accessData<vec4>(0);
	for (size_t i = 0; i < _frustumLines.size(); ++i)
	{
		clr[off + i] = aColor;
		pos[off + i] = _frustumLines[i];
	}

	VertexArray::Description va = _frustumLinesData->generateDescription();
	_frustumGeometry->vertexBuffer()->setData(va.data.binary(), va.data.dataSize());
}