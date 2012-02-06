#include <et/gui/renderingelement.h>

using namespace et;
using namespace et::gui;

RenderingElement::RenderingElement(RenderContext* rc, const std::string& name) : _rs(rc->renderState()),
	_indexArray(new IndexArray(IndexArrayFormat_16bit, 0, IndexArrayContentType_Triangles)), _changed(false)
{
	VertexDeclaration decl(true, Usage_Position, Type_Vec3);
	decl.push_back(Usage_TexCoord0, Type_Vec4);
	decl.push_back(Usage_Color, Type_Vec4);
	
	_vao = rc->vertexBufferFactory().createVertexArrayObject(name + "-vao");
	_vertexBuffer = rc->vertexBufferFactory().createVertexBuffer(name + "-ib", 
		VertexArrayRef(new VertexArray(decl, true)), BufferDrawType_Stream);
	_indexBuffer = rc->vertexBufferFactory().createIndexBuffer(name + "-vb", _indexArray, BufferDrawType_Static);
	_vao->setBuffers(_vertexBuffer, _indexBuffer);
}

void RenderingElement::clear()
{
	_vertexList.setOffset(0);
	_indexArray->setActualSize(0);
	_chunks.clear();
	_changed = true;
}

const VertexArrayObject& RenderingElement::vertexArrayObject()
{
	_rs.bindVertexArray(_vao);

	if (_changed)
	{
		size_t count = _vertexList.currentIndex();
		_vertexBuffer->setData(_vertexList.data(), count * _vertexList.typeSize());
		_indexArray->setActualSize(count);
		_indexBuffer->setData(_indexArray);
		_changed = false;
	}

	return _vao;
}
