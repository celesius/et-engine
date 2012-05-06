/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <assert.h>
#include <et/core/serialization.h>
#include <et/vertexbuffer/vertexdeclaration.h>

using namespace et;

VertexElement VertexDeclaration::_empty;

VertexDeclaration::VertexDeclaration() : _interleaved(true), _totalSize(0)
{
}

VertexDeclaration::VertexDeclaration(bool interleaved) : _interleaved(interleaved), _totalSize(0)
{
}

VertexDeclaration::VertexDeclaration(bool interleaved, VertexAttributeUsage usage, VertexAttributeType type) : 
_interleaved(interleaved), _totalSize(0)
{
	push_back(usage, type);
}

bool VertexDeclaration::has(VertexAttributeUsage usage) const
{
	for (VertexElementList::const_iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		if (i->usage == usage)
			return true;
	}

	return false;
}

bool VertexDeclaration::push_back(VertexAttributeUsage usage, VertexAttributeType type)
{
	return push_back(VertexElement(usage, type, 0, _totalSize));
}

bool VertexDeclaration::push_back(const VertexElement& element)
{
	if (has(element.usage)) return false;

	_totalSize += vertexAttributeTypeSize(element.type);
	_list.push_back(element);

	if (_interleaved)
	{
		for (VertexElementListIterator i = _list.begin(), e = _list.end(); i != e; ++i)
			i->stride = static_cast<int>(_totalSize);
	}

	return true;
}

bool VertexDeclaration::remove(VertexAttributeUsage usage)
{
	for (VertexElementList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		if (i->usage == usage)
		{
			_list.erase(i);
			return true;
		}
	}

	return false;
}

void VertexDeclaration::clear()
{
	_list.clear();
}

const VertexElement& VertexDeclaration::element(size_t i) const
{
	if (i > _list.size())
		return _empty;
	else
		return _list[i];
}

bool VertexDeclaration::operator == (const VertexDeclaration& r) const
{
	if ((r._interleaved != _interleaved) || (_list.size() != r._list.size())) return false;

	VertexElementList::const_iterator si = _list.begin();
	VertexElementList::const_iterator ri = r._list.begin();
	while ((si != _list.end()) && (ri != r._list.end()))
	{
		if ((*si) != (*ri))	return false;
		++si;
		++ri;
	}

	return true;
}

void VertexDeclaration::serialize(std::ostream& stream)
{
	serializeInt(stream, _interleaved);
	serializeInt(stream, static_cast<int>(_totalSize));
	serializeInt(stream, static_cast<int>(_list.size()));
	for (VertexElementList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		serializeInt(stream, i->usage);
		serializeInt(stream, i->type);
		serializeInt(stream, i->stride);
		serializeInt(stream, static_cast<int>(i->offset));
	}
}

void VertexDeclaration::deserialize(std::istream& stream)
{
	_interleaved = deserializeInt(stream) != 0;
	size_t totalSize = static_cast<size_t>(deserializeInt(stream));
	size_t listSize = deserializeInt(stream);
	for (size_t i = 0; i < listSize; ++i)
	{
		VertexAttributeUsage usage = static_cast<VertexAttributeUsage>(deserializeInt(stream));
		VertexAttributeType type = static_cast<VertexAttributeType>(deserializeInt(stream));
		int stride = deserializeInt(stream);
		size_t offset = deserializeInt(stream);
		push_back(VertexElement(usage, type, stride, offset));
	}

	assert(_totalSize == totalSize);
	(void)(totalSize);
}