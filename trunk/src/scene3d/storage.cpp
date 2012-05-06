/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <assert.h>
#include <et/scene3d/storage.h>
#include <et/scene3d/serialization.h>

using namespace et;
using namespace et::s3d;

Scene3dStorage::Scene3dStorage(const std::string& name, Element* parent) : 
	ElementContainer(name, parent), _indexArray(new IndexArray(IndexArrayFormat_16bit, 0, IndexArrayContentType_Triangles))
{
}

void Scene3dStorage::addVertexArray(const VertexArrayRef& va)
{
	_vertexArrays.push_back(va);
}

VertexArrayRef Scene3dStorage::addVertexArrayWithDeclaration(const VertexDeclaration& decl, size_t size)
{
	_vertexArrays.push_back(VertexArrayRef(new VertexArray(decl, size)));
	return _vertexArrays.back();
}

VertexArrayRef Scene3dStorage::vertexArrayWithDeclaration(const VertexDeclaration& decl)
{
	for (VertexArrayList::iterator i = _vertexArrays.begin(), e = _vertexArrays.end(); i != e; ++i)
	{
		if (((*i)->decl() == decl))
			return *i;
	}

	return addVertexArrayWithDeclaration(decl, 0);
}

VertexArrayRef Scene3dStorage::vertexArrayWithDeclarationForAppendingSize(const VertexDeclaration& decl, size_t size)
{
	for (VertexArrayList::iterator i = _vertexArrays.begin(), e = _vertexArrays.end(); i != e; ++i)
	{
		if (((*i)->decl() == decl) && ((*i)->size() + size < IndexArray::MaxShortIndex))
			return *i;
	}

	return addVertexArrayWithDeclaration(decl, 0);
}

int Scene3dStorage::indexOfVertexArray(const VertexArrayRef& va)
{
	int index = 0;

	for (VertexArrayList::iterator i = _vertexArrays.begin(), e = _vertexArrays.end(); i != e; ++i, ++index)
	{
		if (*i == va)
			return index;
	}

	return -1;
}

void Scene3dStorage::serialize(std::ostream& stream)
{
	serializeInt(stream, 0);
}

void Scene3dStorage::deserialize(std::istream& stream, ElementFactory*)
{
	int value = deserializeInt(stream);
	assert(value == 0);
	(void)(value);
}