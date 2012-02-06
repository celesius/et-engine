#pragma once

#include <et/apiobjects/framebuffer.h>

namespace et
{

	class RenderState;
	class FramebufferFactory
	{
	public:
		FramebufferFactory(RenderContext* rc, TextureFactory* tf) : _rc(rc), _tf(tf) { }

		Framebuffer createFramebuffer(const vec2i& size, const std::string& id = "", 
			GLint colorInternalformat = GL_RGBA, GLenum colorFormat = GL_RGBA, GLenum colorType = GL_UNSIGNED_BYTE, 
			GLint depthInternalformat = GL_DEPTH_COMPONENT, GLenum depthFormat = GL_DEPTH_COMPONENT, GLenum depthType = GL_UNSIGNED_INT,
			bool useRenderbuffers = false);

		Framebuffer createCubemapFramebuffer(size_t size, const std::string& id = "",
			GLint colorInternalformat = GL_RGBA, GLenum colorFormat = GL_RGBA, GLenum colorType = GL_UNSIGNED_BYTE, 
			GLint depthInternalformat = GL_DEPTH_COMPONENT, GLenum depthFormat = GL_DEPTH_COMPONENT, GLenum depthType = GL_UNSIGNED_BYTE);

	private:
		FramebufferFactory& operator = (const FramebufferFactory&) { return *this; }

		RenderContext* _rc;
		TextureFactory* _tf;
	};

}