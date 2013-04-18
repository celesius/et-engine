/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/framebuffer.h>

namespace et
{

	class RenderState;
	class FramebufferFactory : public APIObjectFactory
	{
	public:
		FramebufferFactory(RenderContext* rc, TextureFactory* tf) : 
			APIObjectFactory(rc), _tf(tf) { }

		Framebuffer createFramebuffer(const vec2i& size, const std::string& id = "", 
			int colorInternalformat = GL_RGBA, uint32_t colorFormat = GL_RGBA, uint32_t colorType = GL_UNSIGNED_BYTE, 
			int depthInternalformat = GL_DEPTH_COMPONENT, uint32_t depthFormat = GL_DEPTH_COMPONENT, uint32_t depthType = GL_UNSIGNED_INT,
			bool useRenderbuffers = false);

		Framebuffer createCubemapFramebuffer(size_t size, const std::string& id = "",
			int colorInternalformat = GL_RGBA, uint32_t colorFormat = GL_RGBA, uint32_t colorType = GL_UNSIGNED_BYTE, 
			int depthInternalformat = GL_DEPTH_COMPONENT, uint32_t depthFormat = GL_DEPTH_COMPONENT, uint32_t depthType = GL_UNSIGNED_INT);
		
		Framebuffer createFramebufferWrapper(uint32_t fbo, const std::string& id = "");

	private:
		FramebufferFactory& operator = (const FramebufferFactory&)
			{ return *this; }

	private:
		TextureFactory* _tf;
	};

}