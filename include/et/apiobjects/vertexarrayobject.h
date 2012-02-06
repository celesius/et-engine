#pragma once

#include <et/apiobjects/vertexarrayobjectdata.h>

namespace et
{
	class VertexArrayObject : public IntrusivePtr<VertexArrayObjectData>
	{
	private:
		friend class VertexBufferFactory;
		VertexArrayObject(VertexArrayObjectData* data) : IntrusivePtr<VertexArrayObjectData>(data) { }

	public:
		VertexArrayObject() : IntrusivePtr<VertexArrayObjectData>() { }
	};

	typedef std::vector<VertexArrayObject> VertexArrayObjectList;
}