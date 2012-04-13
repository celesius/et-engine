#pragma once

#include <et/core/constants.h>
#include <et/core/containers.h>
#include <et/vertexbuffer/IndexArray.h>
#include <et/vertexbuffer/vertexarray.h>
#include <et/apiobjects/vertexbuffer.h>

namespace et
{

	class Primitives
	{
	public:
		static size_t indexCountForRegularMesh(const vec2i& meshSize, size_t geometryType);

		static IndexType buildTrianglesIndexes(IndexArrayRef buffer, const vec2i& dim, IndexType vertexOffset, size_t indexOffset);
		static IndexType buildTriangleStripIndexes(IndexArrayRef buffer, const vec2i& dim, IndexType vertexOffset, size_t indexOffset);

		static void createSphere(VertexArrayRef data, float radius, const vec2i& density, const vec3& center = vec3(0.0f), 
								const vec2& hemiSphere = vec2(1.0f));
		static void createCylinder(VertexArrayRef data, float radius, float height, const vec2i& density, const vec3& center = vec3(0.0f));

		static void createPlane(VertexArrayRef data, const vec3& normal, const vec2& size, const vec2i& density, 
								const vec3& center = vec3(0.0f), const vec2& texCoordScale = vec2(1.0f), 
								const vec2& texCoordOffset = vec2(0.0f));

		static void createPhotonMap(DataStorage<vec2>& buffer, const vec2i& density);

		static void calculateNormals(VertexArrayRef data, const IndexArrayRef& buffer, size_t first, size_t last);
		static void calculateTangents(VertexArrayRef data, const IndexArrayRef& buffer, size_t first, size_t last);

		static void smoothTangents(VertexArrayRef data, const IndexArrayRef& buffer, size_t first, size_t last);
	};


	#include <et/primitives/primitives.inl.h>

}