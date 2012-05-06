/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/scene3d/element.h>
#include <et/scene3d/storage.h>
#include <et/scene3d/mesh.h>
#include <et/scene3d/cameraelement.h>

namespace et
{
	namespace s3d
	{
		class Scene3d : public ElementContainer, public ElementFactory
		{
		public:
			Scene3d(const std::string& name = "scene");

			void serialize(std::ostream& stream);
			void serialize(const std::string& filename);

			void deserialize(std::istream& stream, RenderContext* rc, TextureCache& tc, CustomElementFactory* factory);
			void deserialize(const std::string& filename, RenderContext* rc, TextureCache& tc, CustomElementFactory* factory);

		private:
			Scene3dStorage::Pointer deserializeStorage(std::istream& stream, RenderContext* rc, TextureCache& tc);
			void buildAPIObjects(Scene3dStorage::Pointer p, RenderContext* rc);

			Element::Pointer createElementOfType(size_t type, Element* parent);
			Material materialWithId(int id);

			VertexBuffer vertexBufferWithId(const std::string& id);
			IndexBuffer indexBufferWithId(const std::string& id);
			VertexArrayObject vaoWithIdentifiers(const std::string& vbid, const std::string& ibid);

		private:
			CustomElementFactory* _externalFactory;
			VertexBufferList _vertexBuffers;
			IndexBufferList _indexBuffers;
			VertexArrayObjectList _vaos;
		};
	}
}