/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/apiobjects/framebufferfactory.h>

using namespace et;

Framebuffer FramebufferFactory::createFramebuffer(const vec2i& size, const std::string& id, GLint colorInternalformat, GLenum colorFormat, 
	GLenum colorType, GLint depthInternalformat, GLenum depthFormat, GLenum depthType, bool useRenderbuffers)
{
	FramebufferDescription desc;
	desc.colorFormat = colorFormat;
	desc.colorInternalformat = colorInternalformat;
	desc.colorType = colorType;
	desc.size = size;
	desc.depthFormat = depthFormat;
	desc.depthInternalformat = depthInternalformat;
	desc.depthType = depthType;
	desc.includeDepthRenderTarget = true;
	desc.numColorRenderTargets = 1;
	desc.colorIsRenderbuffer = useRenderbuffers;
	desc.depthIsRenderbuffer = useRenderbuffers;
	return Framebuffer(new FramebufferData(renderContext(), _tf, desc, id));
}

Framebuffer FramebufferFactory::createCubemapFramebuffer(size_t size, const std::string& id, GLint colorInternalformat, 
	GLenum colorFormat, GLenum colorType, GLint depthInternalformat, GLenum depthFormat, GLenum depthType)
{
	FramebufferDescription desc;
	desc.colorFormat = colorFormat;
	desc.colorInternalformat = colorInternalformat;
	desc.colorType = colorType;
	desc.size = vec2i(size);
	desc.depthFormat = depthFormat;
	desc.depthInternalformat = depthInternalformat;
	desc.depthType = depthType;
	desc.includeDepthRenderTarget = true;
	desc.isCubemap = true;
	desc.numColorRenderTargets = 1;
	return Framebuffer(new FramebufferData(renderContext(), _tf, desc, id));
}

Framebuffer FramebufferFactory::createFramebufferWrapper(GLuint fbo, const std::string& id)
{
	return Framebuffer(new FramebufferData(renderContext(), _tf, fbo, id));
}