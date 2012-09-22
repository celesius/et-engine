/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/core/debug.h>
#include <et/core/serialization.h>
#include <et/vertexbuffer/indexarray.h>
#include <et/primitives/primitives.h>

using namespace et;

const IndexType IndexArray::MaxShortIndex = 65536;
const IndexType IndexArray::MaxSmallIndex = 256;

const int IndexArrayId_1 = 'IAV1';
const int IndexArrayCurrentId = IndexArrayId_1;

IndexArray::IndexArray(IndexArrayFormat format, size_t size, IndexArrayContentType content) : 
_data(size * format), _actualSize(0), _format(format), _contentType(content)
{
	if (content == IndexArrayContentType_Points)
		linearize();
}

void IndexArray::linearize()
{
	for (size_t i = 0; i < capacity(); ++i)
		setIndex(i, i);
}

IndexType IndexArray::getIndex(size_t pos) const
{
	const unsigned char* ptr = _data.element_ptr(pos * _format);

	switch (_format)
	{
	case IndexArrayFormat_16bit:
		return *(reinterpret_cast<const ShortIndexType*>(ptr));

	case IndexArrayFormat_8bit:
		return *(reinterpret_cast<const SmallIndexType*>(ptr));

	default:
		return *(reinterpret_cast<const IndexType*>(ptr));
	}
}

void IndexArray::setIndex(IndexType value, size_t pos)
{
	unsigned char* elementPtr = _data.element_ptr(pos * _format);

	if (_format == IndexArrayFormat_32bit)
	{
		IndexType* ptr = reinterpret_cast<IndexType*>(elementPtr);
		*ptr = value;
	}
	else if (_format == IndexArrayFormat_16bit)
	{
		assert("Index value out of range" && (value < MaxShortIndex));
		ShortIndexType* ptr = reinterpret_cast<ShortIndexType*>(elementPtr);
		*ptr = static_cast<ShortIndexType>(value);
	}
	else if (_format == IndexArrayFormat_8bit)
	{
		assert("Index value out of range" && (value < MaxSmallIndex));
		SmallIndexType* ptr = reinterpret_cast<SmallIndexType*>(elementPtr);
		*ptr = static_cast<SmallIndexType>(value);
	}

	if (pos + 1 > _actualSize)
		_actualSize = pos + 1;
}

void IndexArray::push_back(IndexType value)
{
	setIndex(value, _actualSize++);
}

size_t IndexArray::primitivesCount() const
{
	switch (_contentType)
	{
		case IndexArrayContentType_Points:
			return _actualSize;
			
		case IndexArrayContentType_Lines:
			return _actualSize / 2;
			
		case IndexArrayContentType_Triangles:
			return _actualSize / 3;
			
		case IndexArrayContentType_TriangleStrips:
			return _actualSize - 2;
			
		default:
			break;
	}
	
	return 0;
}

void IndexArray::resize(size_t count)
{
	_data.resize(count * _format);
}

void IndexArray::resizeToFit(size_t count)
{
	_data.fitToSize(count * _format);
}

void IndexArray::compact()
{
	_data.resize(_actualSize * _format);
}

IndexArray::PrimitiveIterator IndexArray::begin() const
{
	return IndexArray::PrimitiveIterator(this, 0);
}

IndexArray::PrimitiveIterator IndexArray::end() const
{
	return IndexArray::PrimitiveIterator(this, capacity());
}

IndexArray::PrimitiveIterator IndexArray::primitive(size_t index) const
{
	size_t primitiveIndex = 0;
	switch (_contentType)
	{
		case IndexArrayContentType_Lines:
		{
			primitiveIndex = 2 * index;
			break;
		}
		case IndexArrayContentType_Triangles:
		{
			primitiveIndex = 3 * index;
			break;
		}
		case IndexArrayContentType_TriangleStrips:
		{
			primitiveIndex = index == 0 ? 0 : (2 + index);
			break;
		}
			
		default:
			primitiveIndex = index;
	}

	return IndexArray::PrimitiveIterator(this, primitiveIndex > capacity() ? capacity() : primitiveIndex);
}

/*
* Supporting types
*/

IndexArray::Primitive::Primitive()
{
	for (size_t i = 0; i < IndexArray::Primitive::MaxVertexCount; ++i)
		index[i] = InvalidIndex;
}

bool IndexArray::Primitive::operator == (const Primitive& p) const
{
	return (p.index[0] == index[0]) && (p.index[1] == index[1]) && (p.index[2] == index[2]);
}

bool IndexArray::Primitive::operator != (const Primitive& p) const
{
	return (p.index[0] != index[0]) || (p.index[1] != index[1]) || (p.index[2] != index[2]);
}

IndexArray::PrimitiveIterator::PrimitiveIterator(const IndexArray* ib, size_t p) : _ib(ib), _pos(p)
{
	configure(_pos);
}

void IndexArray::PrimitiveIterator::configure(size_t p)
{
	IndexType i0 = p;
	IndexType i1 = p;
	IndexType i2 = p;

	if (_ib->contentType() == IndexArrayContentType_Points)
	{
		i1 = InvalidIndex;
		i2 = InvalidIndex;
	}
	else if (_ib->contentType() == IndexArrayContentType_Lines)
	{
		i1 = p + 1;
		i2 = InvalidIndex;
	}
	else if (_ib->contentType() == IndexArrayContentType_Triangles)
	{
		i1 = p + 1;
		i2 = p + 2;
	}
	else if (_ib->contentType() == IndexArrayContentType_TriangleStrips)
	{
		i1 = p + 1;
		i2 = p + 2;
	}

	size_t ibSize = _ib->capacity();
	_primitive[0] = (i0 >= ibSize) ? InvalidIndex : _ib->getIndex(i0);
	_primitive[1] = (i1 >= ibSize) ? InvalidIndex : _ib->getIndex(i1);
	_primitive[2] = (i2 >= ibSize) ? InvalidIndex : _ib->getIndex(i2);
}

IndexArray::PrimitiveIterator& IndexArray::PrimitiveIterator::operator = (const PrimitiveIterator& p)
{
	_ib = p._ib;
	_pos = p._pos;
	_primitive = p._primitive;
	return *this;
}

IndexArray::PrimitiveIterator& IndexArray::PrimitiveIterator::operator ++()
{
	switch (_ib->contentType())
	{
	case IndexArrayContentType_Triangles:
		{
			_pos += 3;
			break;
		}

	default:
		{
			_pos += 1;
			break;
		}
	}

	configure(_pos);

	return *this;
}

IndexArray::PrimitiveIterator IndexArray::PrimitiveIterator::operator ++(int)
{
	IndexArray::PrimitiveIterator temp = *this;
	++(*this);
	return temp;
}

bool IndexArray::PrimitiveIterator::operator == (const IndexArray::PrimitiveIterator& p) const
{
	return _primitive == p._primitive;
}

bool IndexArray::PrimitiveIterator::operator != (const IndexArray::PrimitiveIterator& p) const
{
	return _primitive != p._primitive;
}

void IndexArray::serialize(std::ostream& stream)
{
	serializeInt(stream, IndexArrayCurrentId);
	serializeInt(stream, _format);
	serializeInt(stream, _contentType);
	serializeInt(stream, static_cast<int>(_actualSize));
	serializeInt(stream, static_cast<int>(_data.dataSize()));
	stream.write(_data.binary(), _data.dataSize());
}

void IndexArray::deserialize(std::istream& stream)
{
	int id = deserializeInt(stream);
	if (id == IndexArrayId_1)
	{
		_format = static_cast<IndexArrayFormat>(deserializeInt(stream));
		_contentType = static_cast<IndexArrayContentType>(deserializeInt(stream));
		_actualSize = deserializeInt(stream);
		_data.resize(deserializeInt(stream));
		stream.read(_data.binary(), _data.dataSize());
	}
}