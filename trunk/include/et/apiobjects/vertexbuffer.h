#pragma once

#include <et/apiobjects/vertexbufferdata.h>

namespace et
{
	class VertexBuffer : public IntrusivePtr<VertexBufferData>
	{
	private:
		friend class VertexBufferFactory;
		VertexBuffer(VertexBufferData* data) : IntrusivePtr<VertexBufferData>(data) { }

	public:
		VertexBuffer() : IntrusivePtr<VertexBufferData>(0) { }
	};

	typedef std::vector<VertexBuffer> VertexBufferList;
}