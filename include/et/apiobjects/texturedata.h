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
	public:
		TextureData(RenderContext* rc, const TextureDescription& desc, const std::string& id, bool deferred);
		~TextureData();

		void setWrap(RenderContext* rc, TextureWrap s, TextureWrap t, TextureWrap r = TextureWrap_ClampToEdge);
		void setFiltration(RenderContext* rc, GLenum min_f, GLenum mag_f);
		void compareRefToTexture(RenderContext* rc, bool enable, GLenum compareFunc = GL_LEQUAL);

		vec2 getTexCoord(const vec2& ivec, TextureOrigin origin = TextureOrigin_TopLeft) const;

		void updateData(RenderContext* rc, const TextureDescription& desc);

		inline int glID() const
			{ return _glID; }

		inline int internalFormat() const
			{ return _desc.internalformat; }

		inline int format() const
			{ return _desc.format; }

		inline int dataType() const
			{ return _desc.type; }

		inline int target() const
			{ return _desc.target; }

		inline int width() const
			{ return _desc.size.x; }

		inline GLsizei height() const
			{ return _desc.size.y; }

		inline const vec2i& size() const 
			{ return _desc.size; }

		inline vec2 sizeFloat() const 
			{ return vec2(static_cast<float>(_desc.size.x), static_cast<float>(_desc.size.y)); }

	private:
		void generateTexture(RenderContext* rc);
		void build(RenderContext* rc);
		
	private:
		GLuint _glID;
		TextureDescription _desc;
		vec2 _texel;
	};

}