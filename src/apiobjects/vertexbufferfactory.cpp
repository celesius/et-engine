#include <et/apiobjects/vertexbufferfactory.h>

using namespace et;

VertexBuffer VertexBufferFactory::createVertexBuffer(const std::string& name, VertexArrayRef data, BufferDrawType vertexDrawType)
{
	VertexBuffer vb(new VertexBufferData(_rs, data->getDesc(), vertexDrawType, name));
	vb->setSourceTag(reinterpret_cast<size_t>(data.ptr()));
	return vb;
}

IndexBuffer VertexBufferFactory::createIndexBuffer(const std::string& name, IndexArrayRef data, BufferDrawType drawType)
{
	IndexBuffer ib(new IndexBufferData(_rs, data, drawType, name));
	ib->setSourceTag(reinterpret_cast<size_t>(data.ptr()));
	return ib;
}

VertexArrayObject VertexBufferFactory::createVertexArrayObject(const std::string& name)
{
	return VertexArrayObject(new VertexArrayObjectData(_rs, name));
}

VertexArrayObject VertexBufferFactory::createVertexArrayObject(const std::string& name, VertexBuffer vb, IndexBuffer ib)
{
	return VertexArrayObject(new VertexArrayObjectData(_rs, vb, ib, name));
}
