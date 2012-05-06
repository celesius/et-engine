/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/apiobjects/vertexarrayobject.h>
#include <et/scene3d/element.h>

namespace et
{
	namespace s3d
	{
		class Mesh : public RenderableElement
		{
		public:
			typedef IntrusivePtr<Mesh> Pointer;
			typedef std::map<size_t, Pointer> LodMap;
			static const std::string defaultMeshName;

		public:
			Mesh(const std::string& name = defaultMeshName, Element* parent = 0);

			Mesh(const std::string& name, const VertexArrayObject& ib, const Material& material,
				size_t startIndex, size_t numIndexes, Element* parent = 0);

			ElementType type() const 
				{ return ElementType_Mesh; }

			Mesh* duplicate();

			VertexArrayObject vertexArrayObject();
			VertexBuffer vertexBuffer();
			IndexBuffer indexBuffer();
			const VertexArrayObject vertexArrayObject() const;
			const VertexBuffer vertexBuffer() const;
			const IndexBuffer indexBuffer() const;

			size_t startIndex() const;
			size_t numIndexes() const;
			void setStartIndex(size_t index);
			virtual void setNumIndexes(size_t num);

			void setVertexBuffer(VertexBuffer vb);
			void setIndexBuffer(IndexBuffer ib);
			void setVertexArrayObject(VertexArrayObject vao);

			void serialize(std::ostream& stream);
			void deserialize(std::istream& stream, ElementFactory* factory);

			void cleanupLodChildren();
			void attachLod(size_t level, Mesh::Pointer mesh);

			void setLod(size_t level);

		private:
			Mesh* currentLod();
			const Mesh* currentLod() const;

		private:
			VertexArrayObject _vao;
			LodMap _lods;
			size_t _startIndex;
			size_t _numIndexes;
			size_t _selectedLod;
		};
	}
}