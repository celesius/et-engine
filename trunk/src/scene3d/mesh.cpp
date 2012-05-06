/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/scene3d/mesh.h>
#include <et/scene3d/storage.h>

using namespace et;
using namespace et::s3d;

const std::string Mesh::defaultMeshName = "mesh";

Mesh::Mesh(const std::string& name, Element* parent) : RenderableElement(name, parent), 
	_startIndex(0), _numIndexes(0), _selectedLod(0)
{
}

Mesh::Mesh(const std::string& name, const VertexArrayObject& vao, const Material& material,
	size_t startIndex, size_t numIndexes, Element* parent) : RenderableElement(name, parent), _vao(vao), 
	_startIndex(startIndex), _numIndexes(numIndexes)
{
	setMaterial(material);
}

Mesh* Mesh::duplicate()
{
	Mesh* result = new Mesh(name(), _vao, material(), _startIndex, _numIndexes, parent());
	result->tag = tag;
	return result;
}

void Mesh::setVertexBuffer(VertexBuffer vb)
{
	if (_vao.valid())
		_vao->setVertexBuffer(vb);
}

void Mesh::setIndexBuffer(IndexBuffer ib)
{
	if (_vao.valid())
		_vao->setIndexBuffer(ib);
}

void Mesh::setVertexArrayObject(VertexArrayObject vao)
{
	_vao = vao;
}

void Mesh::serialize(std::ostream& stream)
{
	std::string vbId = (_vao.valid() && _vao->vertexBuffer().valid()) ? intToStr(_vao->vertexBuffer()->sourceTag()) : "0";
	std::string ibId = (_vao.valid() && _vao->indexBuffer().valid()) ? intToStr(_vao->indexBuffer()->sourceTag()) : "0";
	std::string ibName = "ib-" + ibId;
	std::string vbName = "vb-" + vbId;
	std::string vaoName = "vao-" + vbId + "-" + ibId;

	serializeString(stream, vaoName);
	serializeString(stream, vbName);
	serializeString(stream, ibName);
	serializeInt(stream, reinterpret_cast<int>(material().ptr()));

	serializeInt(stream, _startIndex);
	serializeInt(stream, _numIndexes);

	serializeInt(stream, _lods.size());
	for (LodMap::iterator i = _lods.begin(), e = _lods.end(); i != e; ++i)
	{
		serializeInt(stream, i->first);
		i->second->serialize(stream);
	}

	serializeGeneralParameters(stream);
	serializeChildren(stream);
}

void Mesh::deserialize(std::istream& stream, ElementFactory* factory)
{
	std::string vaoName = deserializeString(stream);
	std::string vbName = deserializeString(stream);
	std::string ibName = deserializeString(stream);

	setMaterial(factory->materialWithId(deserializeInt(stream)));
	setVertexArrayObject(factory->vaoWithIdentifiers(vbName, ibName));

	_startIndex = deserializeInt(stream);
	_numIndexes = deserializeInt(stream);

	size_t numLods = deserializeInt(stream);
	for (size_t i = 0; i < numLods; ++i)
	{
		size_t level = deserializeInt(stream);
		Mesh::Pointer p = factory->createElementOfType(ElementType_Mesh, 0);
		p->deserialize(stream, factory);
		attachLod(level, p);
	}

	deserializeGeneralParameters(stream);
	deserializeChildren(stream, factory);
}

void Mesh::attachLod(size_t level, Mesh::Pointer mesh)
{
	_lods[level] = mesh;
	mesh->setActive(false);
}

void Mesh::cleanupLodChildren()
{
	for (LodMap::iterator i = _lods.begin(), e = _lods.end(); i != e; ++i)
	{
		if (i->second->parent() == this)
			i->second->setParent(0);
	}
}

VertexArrayObject Mesh::vertexArrayObject()
{
	return currentLod()->_vao; 
}

const VertexArrayObject Mesh::vertexArrayObject() const
{
	return currentLod()->_vao; 
}

VertexBuffer Mesh::vertexBuffer() 
{
	VertexArrayObject vao = vertexArrayObject();
	return vao.valid() ? vao->vertexBuffer() : VertexBuffer(); 
}

const VertexBuffer Mesh::vertexBuffer() const
{
	VertexArrayObject vao = vertexArrayObject();
	return vao.valid() ? vao->vertexBuffer() : VertexBuffer(); 
}

IndexBuffer Mesh::indexBuffer() 
{
	VertexArrayObject vao = vertexArrayObject();
	return vao.valid() ? vao->indexBuffer() : IndexBuffer(); 
}

const IndexBuffer Mesh::indexBuffer() const
{
	VertexArrayObject vao = vertexArrayObject();
	return vao.valid() ? vao->indexBuffer() : IndexBuffer(); 
}

size_t Mesh::startIndex() const 
{
	return currentLod()->_startIndex; 
}

size_t Mesh::numIndexes() const
{
	return currentLod()->_numIndexes; 
}

void Mesh::setStartIndex(size_t index)
{
	currentLod()->_startIndex = index; 
}

void Mesh::setNumIndexes(size_t num)
{
	currentLod()->_numIndexes = num; 
}

const Mesh* Mesh::currentLod() const
{
	if (_selectedLod == 0) return this;
	LodMap::const_iterator i = _lods.find(_selectedLod);
	return (i == _lods.end()) ? this : i->second.ptr();
}

Mesh* Mesh::currentLod()
{
	if (_selectedLod == 0) return this;
	LodMap::iterator i = _lods.find(_selectedLod);
	return (i == _lods.end()) ? this : i->second.ptr();
}

void Mesh::setLod(size_t level)
{
	LodMap::iterator i = _lods.find(level);
	_selectedLod = (i == _lods.end()) ? 0 : level;
}