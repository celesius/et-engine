/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once 

#include <et/apiobjects/indexbufferdata.h>

namespace et
{
	class IndexBuffer : public IntrusivePtr<IndexBufferData>
	{
	private:
		friend class VertexBufferFactory;
		IndexBuffer(IndexBufferData* data) : IntrusivePtr<IndexBufferData>(data) { }

	public:
		IndexBuffer() : IntrusivePtr<IndexBufferData>(0) { }
	};

	typedef std::vector<IndexBuffer> IndexBufferList;
}