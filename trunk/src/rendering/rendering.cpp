/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/rendering/rendering.h>

namespace et
{

	const std::string VertexAttributeUsageNames[Usage_MAX] = 
	{ 
		"Vertex", "Normal", "Color", "Tangent", "Binormal",
		"TexCoord0", "TexCoord1", "TexCoord2", "TexCoord3"
	};

	VertexAttributeUsage stringToVertexAttribute(const std::string& s)
	{
		for (VertexAttributeUsage i = Usage_Position; i < Usage_MAX; i = (VertexAttributeUsage)(i + 1))
		{
			if (s == VertexAttributeUsageNames[i])
				return i;
		}

		return Usage_Undefined;
	}

	std::string vertexAttributeToString(VertexAttributeUsage va)
	{
		return ((va > Usage_Undefined) && (va < Usage_MAX)) ? VertexAttributeUsageNames[va] : "";
	}

	int vertexAttributeTypeComponents(VertexAttributeType t)
	{
		switch (t)
		{ 
		case Type_Float: 
		case Type_Int:
			return 1;

		case Type_Vec2: 
			return 2;

		case Type_Vec3: 
			return 3;

		case Type_Vec4: 
			return 4;

		default: 
			return 0;
		}
	}

	GLenum vertexAttributeTypeDataType(VertexAttributeType t)
	{
		switch (t)
		{ 
		case Type_Float:
		case Type_Vec2: 
		case Type_Vec3: 
		case Type_Vec4: 
		case Type_Mat3: 
		case Type_Mat4: 
			return GL_FLOAT;

		case Type_Int:
			return GL_INT;

		default: 
			return GL_FLOAT;
		}
	}

	size_t vertexAttributeTypeSize(VertexAttributeType t)
	{
		static size_t floatSize = size_t(sizeof(float));
		static size_t intSize = size_t(sizeof(int));
		
		switch (t)
		{ 
		case Type_Float: 
			return floatSize;

		case Type_Vec2: 
			return 2 * floatSize;

		case Type_Vec3: 
			return 3 * floatSize;

		case Type_Vec4: 
			return 4 * floatSize;

		case Type_Mat3:   
			return 9 * floatSize;

		case Type_Mat4: 
			return 16 * floatSize;

		case Type_Int:
			return intSize;

		default: 
			return 0;
		}
	}

}