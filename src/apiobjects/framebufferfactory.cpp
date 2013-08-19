/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/apiobjects/framebufferfactory.h>

using namespace et;

Framebuffer::Pointer FramebufferFactory::createFramebuffer(const vec2i& size, const std::string& id,
	int32_t colorInternalformat, uint32_t colorFormat, uint32_t colorType, int32_t depthInternalformat,
	uint32_t depthFormat, uint32_t depthType, bool useRenderbuffers)
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
	return Framebuffer::Pointer(new Framebuffer(renderContext(), _tf, desc, id));
}

Framebuffer::Pointer FramebufferFactory::createCubemapFramebuffer(size_t size, const std::string& id,
	int32_t colorInternalformat, uint32_t colorFormat, uint32_t colorType, int32_t depthInternalformat,
	uint32_t depthFormat, uint32_t depthType)
{
	FramebufferDescription desc;
	desc.colorFormat = colorFormat;
	desc.colorInternalformat = colorInternalformat;
	desc.colorType = colorType;
	desc.size = vec2i(static_cast<int>(size & 0xffffffff));
	desc.depthFormat = depthFormat;
	desc.depthInternalformat = depthInternalformat;
	desc.depthType = depthType;
	desc.includeDepthRenderTarget = true;
	desc.isCubemap = true;
	desc.numColorRenderTargets = 1;
	return Framebuffer::Pointer(new Framebuffer(renderContext(), _tf, desc, id));
}

Framebuffer::Pointer FramebufferFactory::createFramebufferWrapper(uint32_t fbo, const std::string& id)
{
	return Framebuffer::Pointer(new Framebuffer(renderContext(), _tf, fbo, id));
}
