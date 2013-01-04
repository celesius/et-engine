/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/vertexbuffer/indexarray.h>
#include <et/vertexbuffer/vertexarray.h>
#include <et/apiobjects/vertexbuffer.h>

namespace et
{

	class Primitives
	{
	public:
		static size_t indexCountForRegularMesh(const vec2i& meshSize, size_t geometryType);

		static IndexType buildTrianglesIndexes(IndexArray::Pointer buffer, const vec2i& dim, IndexType vertexOffset, size_t indexOffset);
		static IndexType buildTriangleStripIndexes(IndexArray::Pointer buffer, const vec2i& dim, IndexType vertexOffset, size_t indexOffset);

		static void createPhotonMap(DataStorage<vec2>& buffer, const vec2i& density);

		static void createSphere(VertexArray::Pointer data, float radius, const vec2i& density, const vec3& center = vec3(0.0f), 
								const vec2& hemiSphere = vec2(1.0f));
		static void createCylinder(VertexArray::Pointer data, float radius, float height, const vec2i& density, const vec3& center = vec3(0.0f));
		static void createTorus(VertexArray::Pointer data, float centralRadius, float sizeRadius, const vec2i& density);
		static void createPlane(VertexArray::Pointer data, const vec3& normal, const vec2& size, const vec2i& density, 
								const vec3& center = vec3(0.0f), const vec2& texCoordScale = vec2(1.0f), 
								const vec2& texCoordOffset = vec2(0.0f));

		static void calculateNormals(VertexArray::Pointer data, const IndexArray::Pointer& buffer, size_t first, size_t last);
		static void calculateTangents(VertexArray::Pointer data, const IndexArray::Pointer& buffer, size_t first, size_t last);
		static void smoothTangents(VertexArray::Pointer data, const IndexArray::Pointer& buffer, size_t first, size_t last);
	};

}