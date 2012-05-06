/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

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