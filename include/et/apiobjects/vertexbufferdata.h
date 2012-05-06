/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/rendering/renderstate.h>
#include <et/apiobjects/apiobject.h>
#include <et/vertexbuffer/vertexarray.h>

namespace et
{
	class VertexBufferData : public APIObjectData
	{
	public:
		VertexBufferData(RenderState& rs, const VertexArray::Description& desc, BufferDrawType vertexDrawType, 
			const std::string& name = std::string());

		~VertexBufferData();
	
		GLuint vertexBuffer() const
			{ return _vertexBuffer; }
		
		size_t vertexCount() const
			{ return _dataSize / _decl.dataSize(); }
		
		const VertexDeclaration& declaration() const
			{ return _decl; }

		void setData(const void* data, size_t dataSize);

		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);

		void setSourceTag(size_t tag)
			{_sourceTag = tag; }

		size_t sourceTag() const
			{ return _sourceTag; }

	private:
		VertexBufferData(RenderState& rs, const VertexDeclaration& decl, const void* vertexData, size_t vertexDataSize, 
			BufferDrawType vertexDrawType, const std::string& name = std::string());

	private:
		RenderState& _rs;
		GLuint _vertexBuffer;
		VertexDeclaration _decl;
		size_t _dataSize;
		size_t _sourceTag;
		BufferDrawType _vertexDrawType;
	};

}