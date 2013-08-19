/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/opengl/opengl.h>
#include <et/apiobjects/apiobjectfactory.h>
#include <et/apiobjects/framebuffer.h>

namespace et
{
	class RenderState;
	
	class FramebufferFactory : public APIObjectFactory
	{
	public:
		FramebufferFactory(RenderContext* rc, TextureFactory* tf) : 
			APIObjectFactory(rc), _tf(tf) { }

		Framebuffer::Pointer createFramebuffer(const vec2i& size, const std::string& id = "",
			int32_t colorInternalformat = GL_RGBA, uint32_t colorFormat = GL_RGBA,
			uint32_t colorType = GL_UNSIGNED_BYTE, int32_t depthInternalformat = GL_DEPTH_COMPONENT,
			uint32_t depthFormat = GL_DEPTH_COMPONENT, uint32_t depthType = GL_UNSIGNED_INT,
			bool useRenderbuffers = false);

		Framebuffer::Pointer createCubemapFramebuffer(size_t size, const std::string& id = "",
			int32_t colorInternalformat = GL_RGBA, uint32_t colorFormat = GL_RGBA,
			uint32_t colorType = GL_UNSIGNED_BYTE, int32_t depthInternalformat = GL_DEPTH_COMPONENT,
			uint32_t depthFormat = GL_DEPTH_COMPONENT, uint32_t depthType = GL_UNSIGNED_INT);

		Framebuffer::Pointer createFramebufferWrapper(uint32_t fbo, const std::string& id = "");

	private:
		ET_DENY_COPY(FramebufferFactory)

	private:
		TextureFactory* _tf;
	};

}