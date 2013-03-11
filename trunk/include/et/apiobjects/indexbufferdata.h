/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/vertexbuffer/indexarray.h>

namespace et
{
	class RenderState;
	class IndexBufferData : public APIObjectData
	{
	public:
		IndexBufferData(RenderState& rs, IndexArray::Pointer i, BufferDrawType drawType, const std::string& name = std::string());
		~IndexBufferData();

		GLenum primitiveType() const
			{ return _primitiveType; }
		
		GLenum dataType() const
			{ return _dataType; }

		void* indexOffset(size_t offset) const;

		GLuint glID() const
			{ return _indexBuffer; }

		size_t size() const
			{ return _size; }

		void setSourceTag(size_t tag)
			{_sourceTag = tag; }

		size_t sourceTag() const
			{ return _sourceTag; }

		void setData(const IndexArray::Pointer& i);

	private:
		void setProperties(const IndexArray::Pointer& i);
		void build(const IndexArray::Pointer& i);

	private:
		RenderState& _rs;
		size_t _size;
		size_t _sourceTag;
		GLuint _indexBuffer;
		GLenum _dataType;
		GLenum _primitiveType;
		BufferDrawType _drawType;
	};
}
