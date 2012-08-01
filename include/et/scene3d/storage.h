/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/vertexbuffer/vertexarray.h>
#include <et/vertexbuffer/indexarray.h>
#include <et/scene3d/material.h>
#include <et/scene3d/element.h>

namespace et
{
	namespace s3d
	{
		class Scene3dStorage : public ElementContainer
		{
		public:
			typedef IntrusivePtr<Scene3dStorage> Pointer;

		public:
			Scene3dStorage(const std::string& name, Element* parent);

			void serialize(std::ostream& stream, SceneVersion version);
			void deserialize(std::istream& stream, ElementFactory* factory, SceneVersion version);

			virtual ElementType type() const 
				{ return ElementType_Storage; }

			VertexArrayList& vertexArrays()
				{ return _vertexArrays; }
			
			IndexArray::Pointer indexArray()
				{ return _indexArray; }

			MaterialList& materials()
				{ return _materials; }

			TextureList& textures()
				{ return _textures; }

			void addTexture(Texture t)
				{ _textures.push_back(t); }

			void addMaterial(Material m)
				{ _materials.push_back(m); }

			void addVertexArray(const VertexArray::Pointer& va);
			VertexArray::Pointer addVertexArrayWithDeclaration(const VertexDeclaration& decl, size_t size);
			VertexArray::Pointer vertexArrayWithDeclaration(const VertexDeclaration& decl);
			VertexArray::Pointer vertexArrayWithDeclarationForAppendingSize(const VertexDeclaration& decl, size_t size);
			int indexOfVertexArray(const VertexArray::Pointer& va);

		private:
			Scene3dStorage* duplicate()
				{ return 0; }

		private:
			VertexArrayList _vertexArrays;
			IndexArray::Pointer _indexArray;
			MaterialList _materials;
			TextureList _textures;
		};
	}
}