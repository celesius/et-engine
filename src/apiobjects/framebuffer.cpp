/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/apiobjects/framebuffer.h>
#include <et/rendering/rendercontext.h>
#include <et/opengl/openglcaps.h>

using namespace et;

extern std::string FramebufferStatusToString(uint32_t status);
extern const uint32_t renderbufferTargets[Framebuffer::MaxRenderTargets];

Framebuffer::Framebuffer(RenderContext* rc, TextureFactory* tf, const FramebufferDescription& desc,
	const std::string& aName) : Object(aName), _isCubemapBuffer(desc.isCubemap != 0), _id(0),
	_size(desc.size), _numTargets(0), _colorRenderbuffer(0), _depthRenderbuffer(0), _rc(rc), _textureFactory(tf)
{
	checkOpenGLError("Framebuffer::Framebuffer %s", name().c_str());

	glGenFramebuffers(1, &_id);
	checkOpenGLError("Framebuffer::Framebuffer -> glGenFramebuffers");

	_rc->renderState().bindFramebuffer(_id);

	bool hasColor = (desc.colorInternalformat != 0) && (desc.numColorRenderTargets > 0);
	bool hasDepth = (desc.depthInternalformat != 0) && desc.includeDepthRenderTarget;
	
	if (hasColor)
	{
		if (desc.colorIsRenderbuffer)
		{
			createColorRenderbuffer(static_cast<uint32_t>(desc.colorInternalformat));
		}
		else 
		{
			for (size_t i = 0; i < desc.numColorRenderTargets; ++i)
			{ 
				Texture c;
				if (_isCubemapBuffer)
				{
					c = tf->genCubeTexture(desc.colorInternalformat, desc.size.x, desc.colorFormat, 
						desc.colorType, name() + "_color_" + intToStr(i));
				}
				else 
				{
					BinaryDataStorage emptyData(desc.size.square() *
						bitsPerPixelForTextureFormat(desc.colorInternalformat, desc.colorType) / 8, 0);
					
					c = tf->genTexture(GL_TEXTURE_2D, desc.colorInternalformat, desc.size, 
						desc.colorFormat, desc.colorType, emptyData, name() + "_color_" + intToStr(i));
				}
				c->setWrap(rc, TextureWrap_ClampToEdge, TextureWrap_ClampToEdge);
				addRenderTarget(c);
			}
		}
	}

	if (hasDepth)
	{
		if (desc.colorIsRenderbuffer)
		{
			createDepthRenderbuffer(static_cast<uint32_t>(desc.depthInternalformat));
		}
		else 
		{
			Texture d;
			if (_isCubemapBuffer && (openGLCapabilites().version() == OpenGLVersion_New))
			{
				d = tf->genCubeTexture(desc.depthInternalformat, desc.size.x, desc.depthFormat, 
					desc.depthType, name() + "_depth");
			}
			else 
			{
				BinaryDataStorage emptyData(desc.size.square() *
					bitsPerPixelForTextureFormat(desc.depthInternalformat, desc.depthType) / 8, 0);
				
				d = tf->genTexture(GL_TEXTURE_2D, desc.depthInternalformat, desc.size,
					desc.depthFormat, desc.depthType, emptyData, name() + "_depth");
			}
			
			if (d.valid())
			{
				d->setWrap(rc, TextureWrap_ClampToEdge, TextureWrap_ClampToEdge);
				setDepthTarget(d);
			}
		}
	}

#if (!ET_OPENGLES)
	if (!hasColor)
	{
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);
	}
#endif
	
	if (hasColor || hasDepth)
		checkStatus();
}

Framebuffer::Framebuffer(RenderContext* rc, TextureFactory* tf, uint32_t fboId,
	const std::string& aName) : Object(aName), _isCubemapBuffer(false), _id(fboId), _numTargets(0),
	_colorRenderbuffer(0), _depthRenderbuffer(0), _rc(rc), _textureFactory(tf)
{
	if (!glIsFramebuffer(fboId)) return;
	
	rc->renderState().bindFramebuffer(fboId);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_size.x);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_size.y);
}

Framebuffer::~Framebuffer()
{
	if (_colorRenderbuffer && glIsRenderbuffer(_colorRenderbuffer))
	{
		glDeleteRenderbuffers(1, &_colorRenderbuffer);
		checkOpenGLError("Framebuffer::~Framebuffer() -> glDeleteRenderbuffers -> color");
	}
	
	if (_depthRenderbuffer && glIsRenderbuffer(_depthRenderbuffer))
	{
		glDeleteRenderbuffers(1, &_depthRenderbuffer);
		checkOpenGLError("Framebuffer::~Framebuffer() -> glDeleteRenderbuffers -> depth");
	}
	
	glDeleteFramebuffers(1, &_id);
	checkOpenGLError("Framebuffer::~Framebuffer -> glDeleteFramebuffers");
	
	_rc->renderState().frameBufferDeleted(_id);
}

bool Framebuffer::checkStatus()
{
	checkOpenGLError("Framebuffer::checkStatus");

	_rc->renderState().bindFramebuffer(_id);
	uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	bool complete = status == GL_FRAMEBUFFER_COMPLETE;

	if (!complete)
		log::error("%s for %s", FramebufferStatusToString(status).c_str(), name().c_str());

	return complete;
}

bool Framebuffer::addRenderTarget(const Texture& rt)
{
	if (!rt.valid() || (rt->size() != _size)) return false;
	assert(glIsTexture(rt->glID()));

	_rc->renderState().bindFramebuffer(_id);

	if (openGLCapabilites().version() == OpenGLVersion_New)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, renderbufferTargets[_numTargets], rt->glID(), 0);
		checkOpenGLError("glFramebufferTexture(...) - %s", name().c_str());
	}
	else
	{
		if (rt->target() == GL_TEXTURE_2D)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, renderbufferTargets[_numTargets], GL_TEXTURE_2D, rt->glID(), 0);
			checkOpenGLError("glFramebufferTexture2D(...) - %s", name().c_str());
		}
		else if (rt->target() == GL_TEXTURE_CUBE_MAP)
		{
			for (GLenum i = 0; i < 6; ++i)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, renderbufferTargets[_numTargets], 
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rt->glID(), 0);
			}
			checkOpenGLError("glFramebufferTexture2D(...) - %s", name().c_str());
		}
	}

	_renderTargets[_numTargets++] = rt;

	return checkStatus();
}

bool Framebuffer::setDepthTarget(const Texture& rt)
{
	if (!rt.valid() || (rt->size() != _size)) return false;

	_rc->renderState().bindFramebuffer(_id);

	if (openGLCapabilites().version() == OpenGLVersion_New)
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, rt->glID(), 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rt->glID(), 0);

	checkOpenGLError("glFramebufferTexture(...) - %s", name().c_str());

	_depthBuffer = rt;
	return checkStatus();
}

bool Framebuffer::setDepthTarget(const Texture& texture, uint32_t target)
{
	if (!texture.valid() || (texture->width() != _size.x) || (texture->height() != _size.y)) return false;

	_rc->renderState().bindFramebuffer(_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture->glID(), 0);
	checkOpenGLError("glFramebufferTexture2D(...) - %s", name().c_str());

	return checkStatus();
}

void Framebuffer::addSameRendertarget()
{
	if (_numTargets == 0) return;

	Texture& prev = _renderTargets[_numTargets - 1];

	std::string texName = name() + "_color_" + intToStr(_numTargets);
	
	Texture c;
	if (_isCubemapBuffer)
	{
		c = _textureFactory->genCubeTexture(prev->internalFormat(), prev->width(), 
			prev->format(), prev->dataType(), texName);
	}
	else
	{
		BinaryDataStorage emptyData(prev->size().square() *
			bitsPerPixelForTextureFormat(prev->internalFormat(), prev->dataType()) / 8, 0);
		
		c = _textureFactory->genTexture(prev->target(), prev->internalFormat(),
			prev->size(), prev->format(), prev->dataType(), emptyData, texName);
	}
	
	c->setWrap(_rc, TextureWrap_ClampToEdge, TextureWrap_ClampToEdge);
	addRenderTarget(c);
}

bool Framebuffer::setCurrentRenderTarget(const Texture& texture)
{
	assert(texture.valid());
	
	if (texture == _currentRendertarget) return true;
	
	_currentRendertarget = texture;
	_rc->renderState().bindFramebuffer(_id);

	if (openGLCapabilites().version() == OpenGLVersion_New)
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->glID(), 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->target(), texture->glID(), 0);
	
	checkOpenGLError("Framebuffer::setCurrentRenderTarget");
	return checkStatus();
}

bool Framebuffer::setCurrentRenderTarget(const Texture& texture, uint32_t target)
{
	assert(texture.valid());
	_rc->renderState().bindFramebuffer(_id);

	if (openGLCapabilites().version() == OpenGLVersion_New)
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->glID(), 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture->glID(), 0);

	checkOpenGLError("Framebuffer::setCurrentRenderTarget");
	return checkStatus();
}

bool Framebuffer::setCurrentRenderTarget(size_t index)
{
	assert(index < _numTargets);
	assert(_renderTargets[index].valid());

	return setCurrentRenderTarget(_renderTargets[index]);
}

#if (ET_OPENGLES)
void Framebuffer::setDrawBuffersCount(int)
{
	assert(false && "glDrawBuffers is not supported in OpenGL ES");
}
#else
void Framebuffer::setDrawBuffersCount(int count)
{
	_rc->renderState().bindFramebuffer(_id);
	glDrawBuffers(count, renderbufferTargets);
	checkOpenGLError("Framebuffer::setDrawBuffersCount -> glDrawBuffers - %s", name().c_str());
	checkStatus();
}
#endif

bool Framebuffer::setCurrentCubemapFace(uint32_t faceIndex)
{
	if (!_isCubemapBuffer) return false;

	_rc->renderState().bindFramebuffer(_id);

	uint32_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex;
	
	if (_renderTargets[0].valid())
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, _renderTargets[0]->glID(), 0);
		checkOpenGLError("setCurrentCubemapFace -> color");
	}
	
	if (_depthBuffer.valid())
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, _depthBuffer->glID(), 0);
		checkOpenGLError("setCurrentCubemapFace -> depth");
	}

	return checkStatus();
}

void Framebuffer::createColorRenderbuffer(uint32_t internalFormat)
{
	glGenRenderbuffers(1, &_colorRenderbuffer);
	checkOpenGLError("glGenRenderbuffers -> color");
	
	_rc->renderState().bindRenderbuffer(_colorRenderbuffer);
	
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, _size.x, _size.y);
	checkOpenGLError("glRenderbufferStorage -> color");
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderbuffer);
	checkOpenGLError("glFramebufferRenderbuffer -> color");
}

void Framebuffer::createDepthRenderbuffer(uint32_t internalFormat)
{
	glGenRenderbuffers(1, &_depthRenderbuffer);
	checkOpenGLError("glGenRenderbuffers -> depth");
	
	_rc->renderState().bindRenderbuffer(_depthRenderbuffer);
	
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, _size.x, _size.y);
	checkOpenGLError("glRenderbufferStorage -> depth");
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);
	checkOpenGLError("glFramebufferRenderbuffer -> depth");
}

void Framebuffer::forceSize(const vec2i& sz)
{
	_size = sz;
}

/*
 * Support
 */
std::string FramebufferStatusToString(uint32_t status)
{
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE:
			return "GL_FRAMEBUFFER_COMPLETE";
			
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return "GL_FRAMEBUFFER_UNSUPPORTED";
			
#if defined(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif
			
#if defined(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
#endif
			
#if defined(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
#endif
			
#if defined(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
#endif
			
		default:
			return "Unknown FBO status " + intToStr(status);
	}
}

const uint32_t renderbufferTargets[Framebuffer::MaxRenderTargets] =
{
	GL_COLOR_ATTACHMENT0,
	
#if defined(GL_COLOR_ATTACHMENT1)
	GL_COLOR_ATTACHMENT1,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT2)
	GL_COLOR_ATTACHMENT2,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT3)
	GL_COLOR_ATTACHMENT3,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT4)
	GL_COLOR_ATTACHMENT4,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT5)
	GL_COLOR_ATTACHMENT5,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT6)
	GL_COLOR_ATTACHMENT6,
#else
	GL_COLOR_ATTACHMENT0,
#endif
	
#if defined(GL_COLOR_ATTACHMENT7)
	GL_COLOR_ATTACHMENT7,
#else
	GL_COLOR_ATTACHMENT0,
#endif
};