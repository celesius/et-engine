/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

namespace et
{
	enum BufferDrawType
	{
		BufferDrawType_Static,
		BufferDrawType_Dynamic,
		BufferDrawType_Stream,
		BufferDrawType_max
	};

	enum TextureWrap
	{
		TextureWrap_Repeat,
		TextureWrap_ClampToEdge,
		TextureWrap_MirrorRepeat,
		TextureWrap_max
	};

	enum TextureFiltration
	{
		TextureFiltration_Nearest,
		TextureFiltration_Linear,
		TextureFiltration_NearestMipMapNearest,
		TextureFiltration_LinearMipMapNearest,
		TextureFiltration_NearestMipMapLinear,
		TextureFiltration_LinearMipMapLinear,
		TextureFiltration_max
	};
	
	enum PrimitiveType
	{
		PrimitiveType_Points,
		PrimitiveType_Lines,
		PrimitiveType_Triangles,
		PrimitiveType_TriangleStrips,
		PrimitiveType_LineStrip,
		PrimitiveType_max
	};

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
	
	enum IndexArrayFormat : size_t
	{
		IndexArrayFormat_Undefined  = 0,
		IndexArrayFormat_8bit = 1,
		IndexArrayFormat_16bit = 2,
		IndexArrayFormat_32bit = 4,
		IndexArrayFormat_max
	};

	typedef unsigned int IndexType;
	typedef unsigned short ShortIndexType;
	typedef unsigned char SmallIndexType;
	
	enum
	{
		MaxTextureUnits	= 8,
		InvalidIndex = static_cast<IndexType>(-1)
	};

}