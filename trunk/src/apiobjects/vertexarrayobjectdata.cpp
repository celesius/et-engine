#include <et/rendering/renderstate.h>
#include <et/apiobjects/vertexarrayobjectdata.h>

using namespace et;

VertexArrayObjectData::VertexArrayObjectData(RenderState& rs, VertexBuffer vb, IndexBuffer ib, const std::string& name) : 
	APIObjectData(name), _rs(rs), _vb(vb), _ib(ib), _vao(0)
{
	init();
}

VertexArrayObjectData::VertexArrayObjectData(RenderState& rs, const std::string& name) : APIObjectData(name), _rs(rs), _vao(0)
{
	init();
}

VertexArrayObjectData::~VertexArrayObjectData()
{
	if (_vao && glIsVertexArray(_vao))
		glDeleteVertexArrays(1, &_vao);

	_rs.vertexArrayDeleted(_vao);
}

void VertexArrayObjectData::init()
{
	glGenVertexArrays(1, &_vao);
	checkOpenGLError("glGenVertexArrays in " + name());

	_rs.bindVertexArray(_vao);
	_rs.bindBuffers(_vb, _ib, true);
}

void VertexArrayObjectData::setBuffers(VertexBuffer vb, IndexBuffer ib)
{
	_vb = vb;
	_ib = ib;
	_rs.bindVertexArray(_vao);
	_rs.bindBuffers(_vb, _ib, true);
}

void VertexArrayObjectData::setVertexBuffer(VertexBuffer vb)
{
	_vb = vb;
	_rs.bindVertexArray(_vao);
	_rs.bindBuffers(_vb, _ib, true);
}

void VertexArrayObjectData::setIndexBuffer(IndexBuffer ib)
{
	_ib = ib;
	_rs.bindVertexArray(_vao);
	_rs.bindBuffers(_vb, _ib, true);
}
