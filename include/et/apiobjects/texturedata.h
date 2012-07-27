/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/autoptr.h>
#include <et/core/containers.h>
#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/texturedescription.h>

namespace et
{
	class RenderContext;

	class TextureData : public APIObjectData
	{
	private:
		friend class TextureFactory;
		
		TextureData(RenderContext* rc, const TextureDescription& desc, const std::string& id, bool deferred);
		TextureData(RenderContext* rc, GLuint texture, const vec2i& size, const std::string& name);
		
	public:
		~TextureData();

		void setWrap(RenderContext* rc, TextureWrap s, TextureWrap t, TextureWrap r = TextureWrap_ClampToEdge);
		void setFiltration(RenderContext* rc, TextureFiltration minFiltration, TextureFiltration magFiltration);
		void setMaxLod(RenderContext* rc, size_t value);
		
		void compareRefToTexture(RenderContext* rc, bool enable, GLenum compareFunc = GL_LEQUAL);
		void generateMipMaps(RenderContext* rc);

		vec2 getTexCoord(const vec2& ivec, TextureOrigin origin = TextureOrigin_TopLeft) const;

		void updateData(RenderContext* rc, const TextureDescription& desc);
		void updateDataDirectly(RenderContext* rc, const vec2i& size, char* data, size_t dataSize);

		int glID() const
			{ return _glID; }

		int internalFormat() const
			{ return _desc.internalformat; }

		int format() const
			{ return _desc.format; }

		int dataType() const
			{ return _desc.type; }

		int target() const
			{ return _desc.target; }

		int width() const
			{ return _desc.size.x; }

		GLsizei height() const
			{ return _desc.size.y; }

		const vec2i& size() const 
			{ return _desc.size; }

		vec2 sizeFloat() const 
			{ return vec2(static_cast<float>(_desc.size.x), static_cast<float>(_desc.size.y)); }

		const vec2& texel() const
			{ return _texel; }

	private:
		void generateTexture(RenderContext* rc);
		void build(RenderContext* rc);
        void buildData(char* ptr, size_t dataSize);
		
	private:
		GLuint _glID;
		TextureDescription _desc;
		vector3<TextureWrap> _wrap;
		vector2<TextureFiltration> _filtration;
		vec2 _texel;
		bool _own;
	};

}