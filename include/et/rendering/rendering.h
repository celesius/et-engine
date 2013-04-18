/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/opengl/opengl.h>

namespace et
{

	enum VertexAttributeUsage
	{
		Usage_Undefined = -1,

		Usage_Position,
		Usage_Normal,
		Usage_Color,
		Usage_Tangent,
		Usage_Binormal,
		Usage_TexCoord0,
		Usage_TexCoord1,
		Usage_TexCoord2,
		Usage_TexCoord3,
		Usage_Smoothing,

		Usage_max
	};

	enum VertexAttributeType
	{
		Type_Undefined = -1,

		Type_Float = GL_FLOAT,
		Type_Vec2 = GL_FLOAT_VEC2,
		Type_Vec3 = GL_FLOAT_VEC3,
		Type_Vec4 = GL_FLOAT_VEC4,
		Type_Mat3 = GL_FLOAT_MAT3,
		Type_Mat4 = GL_FLOAT_MAT4,
		Type_Int = GL_INT
	};

	size_t vertexAttributeUsageMask(VertexAttributeUsage u);

	VertexAttributeUsage stringToVertexAttribute(const std::string& s);
	std::string vertexAttributeToString(VertexAttributeUsage va);

	size_t vertexAttributeTypeSize(VertexAttributeType t);
	int vertexAttributeTypeComponents(VertexAttributeType t);
	uint32_t vertexAttributeTypeDataType(VertexAttributeType t);

}
