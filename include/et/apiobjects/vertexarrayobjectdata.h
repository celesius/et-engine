#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/vertexbuffer.h>
#include <et/apiobjects/indexbuffer.h>

namespace et
{
	class RenderState;
	class VertexArrayObjectData : public APIObjectData
	{
	public:
		VertexArrayObjectData(RenderState& rs, VertexBuffer vb, IndexBuffer ib, const std::string& name = std::string());
		VertexArrayObjectData(RenderState& rs, const std::string& name = std::string());
		~VertexArrayObjectData();

		inline GLuint vertexArrayObject() const
			{ return _vao; }

		VertexBuffer& vertexBuffer() 
			{ return _vb; };
		const VertexBuffer& vertexBuffer() const
			{ return _vb; };

		IndexBuffer& indexBuffer()
			{ return _ib; };
		const IndexBuffer& indexBuffer() const
			{ return _ib; };

		void setVertexBuffer(VertexBuffer ib);
		void setIndexBuffer(IndexBuffer ib);
		void setBuffers(VertexBuffer vb, IndexBuffer ib);

	private:
		void init();

	private:
		RenderState& _rs;
		VertexBuffer _vb;
		IndexBuffer _ib;
		GLuint _vao;
	};
}