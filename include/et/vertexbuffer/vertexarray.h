/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/vertexbuffer/vertexdeclaration.h>
#include <et/vertexbuffer/vertexdatachunk.h>

namespace et
{ 
	class VertexArray : public Shared
	{
	public:
		struct Description
		{
			VertexDeclaration declaration;
			BinaryDataStorage data;
		};

		int tag;

	public:
		VertexArray();
		VertexArray(const VertexDeclaration& decl, size_t size);
		
		VertexDataChunk& smoothing()
			{ return _smoothing; }

		const VertexDataChunk& smoothing() const
			{ return _smoothing; }

		VertexDataChunk chunk(VertexAttributeUsage usage);
		Description getDesc() const;
		
		void increase(size_t size);
		void fitToSize(size_t size);
		
		size_t size() const
			{ return _size; }

		const VertexDeclaration decl() const
			{ return _decl; }

		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);
		
	private:
		size_t _size;
		VertexDeclaration _decl;
		VertexDataChunkList _chunks;
		VertexDataChunk _smoothing;
	};
	
	typedef et::IntrusivePtr<VertexArray> VertexArrayRef;
	typedef std::vector<VertexArrayRef> VertexArrayList;
}