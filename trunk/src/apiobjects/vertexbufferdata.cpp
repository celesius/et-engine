/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/apiobjects/vertexbufferdata.h>

using namespace et;

VertexBufferData::VertexBufferData(RenderState& rs, const VertexArray::Description& desc, BufferDrawType vertexDrawType, 
	const std::string& name) : APIObjectData(name), _rs(rs), _vertexBuffer(0), _decl(desc.declaration), 
	_dataSize(desc.data.dataSize()), _sourceTag(0), _vertexDrawType(vertexDrawType)
{
	glGenBuffers(1, &_vertexBuffer);
	setData(desc.data.data(), desc.data.dataSize());
}

VertexBufferData::VertexBufferData(RenderState& rs, const VertexDeclaration& decl, const void* vertexData, size_t vertexDataSize, 
	BufferDrawType vertexDrawType, const std::string& name) : APIObjectData(name), _rs(rs), _vertexBuffer(0), 
	_decl(decl), _dataSize(vertexDataSize), _sourceTag(0), _vertexDrawType(vertexDrawType)
{
	glGenBuffers(1, &_vertexBuffer);
	setData(vertexData, vertexDataSize);
}

VertexBufferData::~VertexBufferData()
{
	if (_vertexBuffer && glIsBuffer(_vertexBuffer))
		glDeleteBuffers(1, &_vertexBuffer);

	_rs.vertexBufferDeleted(_vertexBuffer);
}

void VertexBufferData::setData(const void* data, size_t dataSize)
{
	_dataSize = dataSize;
	GLenum vertexDraw = _vertexDrawType == BufferDrawType_Stream ? GL_STREAM_DRAW : GL_STATIC_DRAW;

	_rs.bindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _dataSize, data, vertexDraw);

	checkOpenGLError("glBufferData(GL_ARRAY_BUFFER, ...)");
}

void VertexBufferData::serialize(std::ostream&)
{

}

void VertexBufferData::deserialize(std::istream&)
{

}