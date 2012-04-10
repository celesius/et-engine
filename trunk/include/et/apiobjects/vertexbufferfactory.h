#pragma once

#include <et/apiobjects/vertexarrayobject.h>

namespace et
{

	class RenderState;
	class VertexDeclaration;

	class VertexBufferFactory
	{
	public:
		VertexBufferFactory(RenderState& rs) : _rs(rs) { }

		VertexBuffer createVertexBuffer(const std::string& name, VertexArrayRef data, BufferDrawType drawType);
		IndexBuffer createIndexBuffer(const std::string& name, IndexArrayRef data, BufferDrawType drawType);

		VertexArrayObject createVertexArrayObject(const std::string& name);

	private:
		VertexArrayObject createVertexArrayObject(const std::string& name, VertexBuffer vb, IndexBuffer ib);

	private:
		VertexBufferFactory(const VertexBufferFactory& r) : _rs(r._rs)
			{ }

		VertexBufferFactory& operator = (const VertexBufferFactory&)
			{ return *this; }

		RenderState& _rs;
	};

}