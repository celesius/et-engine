/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/rendering/renderstate.h>
#include <et/apiobjects/indexbufferdata.h>

using namespace et;

IndexBufferData::IndexBufferData(RenderState& rs, IndexArray::Pointer i, BufferDrawType drawType, const std::string& name) : 
	APIObjectData(name), _rs(rs), _size(i->actualSize()), _sourceTag(0), _indexBuffer(0), _dataType(0), _geometryType(0), _drawType(drawType)
{
	build(i);
}

IndexBufferData::~IndexBufferData()
{
	if ((_indexBuffer != 0) && glIsBuffer(_indexBuffer))
		glDeleteBuffers(1, &_indexBuffer);

	_rs.indexBufferDeleted(_indexBuffer);
}

void IndexBufferData::setProperties(const IndexArray::Pointer& i)
{
	_size = i->actualSize();

	switch (i->format())
	{
	case IndexArrayFormat_8bit:
		{
			_dataType = GL_UNSIGNED_BYTE;
			break;
		}
	case IndexArrayFormat_16bit:
		{
			_dataType = GL_UNSIGNED_SHORT;
			break;
		}
	case IndexArrayFormat_32bit:
		{
			_dataType = GL_UNSIGNED_INT;
			break;
		}
	default:
		{
			std::cout << "Unknown IndexArray format" << std::endl;
		}
	}

	switch (i->contentType())
	{
	case IndexArrayContentType_Lines:
		{
			_geometryType = GL_LINES;
			break;
		}

	case IndexArrayContentType_Triangles:
		{
			_geometryType = GL_TRIANGLES;
			break;
		}

	case IndexArrayContentType_TriangleStrips:
		{
			_geometryType = GL_TRIANGLE_STRIP;
			break;
		}

	default:
		_geometryType = GL_POINTS;
	}
}

void IndexBufferData::build(const IndexArray::Pointer& i)
{
	setProperties(i);

	GLenum indexDraw = _drawType == BufferDrawType_Stream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	size_t indexDataSize = i->format() * _size;

	if (!_indexBuffer || !glIsBuffer(_indexBuffer))
	{
		glGenBuffers(1, &_indexBuffer);
		checkOpenGLError("glGenBuffers(1, &_indexBuffer)");
	}

	_rs.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, i->data(), indexDraw);
	checkOpenGLError("glBufferData(GL_ELEMENT_ARRAY_BUFFER, ....)");
}

void* IndexBufferData::indexOffset(size_t offset) const
{
	switch (_dataType)
	{
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
		return reinterpret_cast<void*>(offset);

	case GL_UNSIGNED_SHORT:
	case GL_SHORT:
		return reinterpret_cast<void*>(sizeof(short) * offset);

	case GL_UNSIGNED_INT:
	case GL_INT:
		return reinterpret_cast<void*>(sizeof(int) * offset);

	default: 
		return 0;
	}
}

void IndexBufferData::setData(const IndexArray::Pointer& i)
{
	build(i);
}
