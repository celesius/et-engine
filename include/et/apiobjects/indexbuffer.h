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