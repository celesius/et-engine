#pragma once

#include <et/scene3d/mesh.h>
#include <et/collision/collision.h>

namespace et
{
	namespace s3d
	{
		class CollisionMesh : public Mesh
		{
		public:
			typedef IntrusivePtr<CollisionMesh> Pointer;
			typedef DataStorage<triangle> CollisionData;

		public:
			CollisionMesh(const std::string& name = defaultMeshName, Element* parent = 0);
			CollisionMesh(const std::string& name, const VertexArrayObject& ib, const Material& material,
				size_t startIndex, size_t numIndexes, Element* parent = 0);

			ElementType type() const 
				{ return ElementType_CollisionMesh; }

			void setNumIndexes(size_t num);
			void fillCollisionData(VertexArrayRef v, IndexArrayRef i);

			CollisionMesh* duplicate();

			Sphere sphere();
			AABB aabb();
			OBB obb();

			const CollisionData& triangles() const
				{ return _data; }

			void serialize(std::ostream& stream);
			void deserialize(std::istream& stream, ElementFactory* factory);

			mat4 finalTransform();
			mat4 finalTransformInverse();
			float finalTransformScale();

		private:
			void buildInverseTransform();

		private:
			mat4 _cachedInverseTransform;
			CollisionData _data;
			vec3 _size;
			vec3 _center;
			float _radius;
			float _cachedFinalTransformScale;
			bool _inverseTransformValid;
		};
	}
}