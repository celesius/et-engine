/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/core/tools.h>
#include <et/apiobjects/framebufferdata.h>
#include <et/rendering/rendercontext.h>

using namespace et;

inline std::string FramebufferStatusToString(GLenum status)
{
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		return "GL_FRAMEBUFFER_COMPLETE"; 

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; 

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; 

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

static const GLenum RENDERBUFFERS_TARGETS[FramebufferData::MaxRenderTargets] = 
{ 
	GL_COLOR_ATTACHMENT0, 
#if (ET_OPENGLES)
	0, 0, 0, 
	0, 0, 0, 
	0, 0, 0, 
	0, 0, 0,
	0, 0, 0
#else	
	GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15 
#endif	
};

FramebufferData::FramebufferData(RenderContext* rc, TextureFactory* tf, const FramebufferDescription& desc, const std::string& aName) : APIObjectData(aName), 
	_isCubemapBuffer(desc.isCubemap != 0), _id(0), _size(desc.size), _numTargets(0), 
	_colorRenderbuffer(0), _depthRenderbuffer(0),
	_rc(rc), _textureFactory(tf)
{
	checkOpenGLError("Framebuffer::Framebuffer " + name());

	glGenFramebuffers(1, &_id);
	checkOpenGLError("Framebuffer::Framebuffer -> glGenFramebuffers");

	_rc->renderState().bindFramebuffer(_id);

	bool hasColor = (desc.colorInternalformat != 0) && (desc.numColorRenderTargets > 0);
	bool hasDepth = (desc.depthInternalformat != 0) && desc.includeDepthRenderTarget;
	
	if (hasColor)
	{
		if (desc.colorIsRenderbuffer)
		{
			createColorRenderbuffer(desc.colorInternalformat);
		}
		else 
		{
			for (int i = 0; i < desc.numColorRenderTargets; ++i)
			{ 
				Texture c;
				if (_isCubemapBuffer)
				{
					c = tf->genCubeTexture(desc.colorInternalformat, desc.size.x, desc.colorFormat, 
						desc.colorType, name() + "_color_" + intToStr(i));
				}
				else 
				{
					c = tf->genTexture(GL_TEXTURE_2D, desc.colorInternalformat, desc.size, 
						desc.colorFormat, desc.colorType, BinaryDataStorage(), name() + "_color_" + intToStr(i));
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
			createDepthRenderbuffer(desc.depthInternalformat);
		}
		else 
		{
			Texture d;
			if (_isCubemapBuffer)
			{
				d = tf->genCubeTexture(desc.depthInternalformat, desc.size.x, desc.depthFormat, 
					desc.depthType, name() + "_depth");
			}
			else 
			{
				d = tf->genTexture(GL_TEXTURE_2D, desc.depthInternalformat, desc.size, 
					desc.depthFormat, desc.depthType, BinaryDataStorage(), name() + "_depth");
			}
			d->setWrap(rc, TextureWrap_ClampToEdge, TextureWrap_ClampToEdge);
			setDepthTarget(d);
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
		check();
}

FramebufferData::FramebufferData(RenderContext* rc, TextureFactory* tf, GLuint fboId, const std::string& aName) : APIObjectData(aName), 
	_isCubemapBuffer(false), _id(fboId), _numTargets(0), _colorRenderbuffer(0), _depthRenderbuffer(0), _rc(rc), _textureFactory(tf)
{
	if (!glIsFramebuffer(fboId)) return;
	
	rc->renderState().bindFramebuffer(fboId);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_size.x);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_size.y);
}

FramebufferData::~FramebufferData()
{
	if (_colorRenderbuffer && glIsRenderbuffer(_colorRenderbuffer))
	{
		glDeleteRenderbuffers(1, &_colorRenderbuffer);
		checkOpenGLError("FramebufferData::~FramebufferData() -> glDeleteRenderbuffers -> color");
	}
	
	if (_depthRenderbuffer && glIsRenderbuffer(_depthRenderbuffer))
	{
		glDeleteRenderbuffers(1, &_depthRenderbuffer);
		checkOpenGLError("FramebufferData::~FramebufferData() -> glDeleteRenderbuffers -> depth");
	}
	
	glDeleteFramebuffers(1, &_id);
	checkOpenGLError("FramebufferData::~FramebufferData -> glDeleteFramebuffers");
	
	_rc->renderState().frameBufferDeleted(_id);
}

bool FramebufferData::check()
{
	static const std::string tag = "FramebufferData::check";
	checkOpenGLError(tag);
	(void)tag;

	_rc->renderState().bindFramebuffer(_id);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << FramebufferStatusToString(status) + " for " + name() << std::endl;

	return (status == GL_FRAMEBUFFER_COMPLETE);
}

bool FramebufferData::addRenderTarget(const Texture& rt)
{
	if (!rt.valid() || (rt->size() != _size)) return false;

	_rc->renderState().bindFramebuffer(_id);

#if (ET_OPENGLES)
    if (rt->target() == GL_TEXTURE_2D)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _numTargets, GL_TEXTURE_2D, rt->glID(), 0);
        checkOpenGLError("Framebuffer::addRenderTarget -> glFramebufferTexture(..., GL_COLOR_ATTACHMENT0 " + name());
    }
    else if (rt->target() == GL_TEXTURE_CUBE_MAP)
    {
        for (size_t i = 0; i < 6; ++i)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _numTargets, 
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rt->glID(), 0);
        }
        checkOpenGLError("Framebuffer::addRenderTarget -> glFramebufferTexture(..., GL_COLOR_ATTACHMENT0 " + name());
    }
#else	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _numTargets, rt->glID(), 0);
	checkOpenGLError("Framebuffer::addRenderTarget -> glFramebufferTexture(..., GL_COLOR_ATTACHMENT0 " + name());
#endif	


	_renderTargets[_numTargets++] = rt;

	return check();
}

bool FramebufferData::setDepthTarget(const Texture& rt)
{
	if (!rt.valid() || (rt->size() != _size)) return false;

	_rc->renderState().bindFramebuffer(_id);

#if (ET_OPENGLES)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rt->glID(), 0);
#else	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, rt->glID(), 0);
#endif	

	checkOpenGLError("Framebuffer::setDepthTarget -> glFramebufferTexture(..., GL_DEPTH_ATTACHMENT " + name());

	_depthBuffer = rt;
	return check();
}

bool FramebufferData::setDepthTarget(const Texture& texture, GLenum target)
{
	if (!texture.valid() || (texture->width() != _size.x) || (texture->height() != _size.y)) return false;

	_rc->renderState().bindFramebuffer(_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture->glID(), 0);
	checkOpenGLError("Framebuffer::setDepthTarget -> glFramebufferTexture2D(..., GL_DEPTH_ATTACHMENT " + name());

	return check();
}

void FramebufferData::addSameRendertarget()
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
		c = _textureFactory->genTexture(prev->target() , prev->internalFormat(), 
			prev->size(), prev->format(), prev->dataType(), BinaryDataStorage(), texName);
	}
	
	c->setWrap(_rc, TextureWrap_ClampToEdge, TextureWrap_ClampToEdge);
	addRenderTarget(c);
}

bool FramebufferData::setCurrentRenderTarget(const Texture& texture)
{
	_rc->renderState().bindFramebuffer(_id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->target(), texture->glID(), 0);
	checkOpenGLError("Framebuffer::setCurrentRenderTarget -> glFramebufferTexture2D(..., GL_COLOR_ATTACHMENT0 " + name());

	return check();
}

bool FramebufferData::setCurrentRenderTarget(const Texture& texture, GLenum target)
{
	_rc->renderState().bindFramebuffer(_id);

#if (ET_OPENGLES)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture->glID(), 0);
#else
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->glID(), 0);
#endif

	checkOpenGLError("Framebuffer::setCurrentRenderTarget -> glFramebufferTexture2D(..., GL_COLOR_ATTACHMENT0 " + name());

	return check();
}

bool FramebufferData::setCurrentRenderTarget(GLenum rt)
{
	return (rt < _numTargets) ? setCurrentRenderTarget(_renderTargets[rt]) : false;
}

void FramebufferData::setDrawBuffersCount(int count)
{
#if (!ET_OPENGLES)
	_rc->renderState().bindFramebuffer(_id);
	glDrawBuffers(count, RENDERBUFFERS_TARGETS);
	checkOpenGLError("Framebuffer::setDrawBuffersCount -> glDrawBuffers " + name());
	check();
#else
	std::cout << "FramebufferData::setDrawBuffersCount(" << count << ") call in OpenGL ES" << std::endl;
#endif	
}

bool FramebufferData::setCurrentCubemapFace(size_t faceIndex)
{
	if (!_isCubemapBuffer) return false;

	GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex;

	_rc->renderState().bindFramebuffer(_id);
   
    if (_renderTargets[0].valid())
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, _renderTargets[0]->glID(), 0);

    if (_depthBuffer.valid())
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, _depthBuffer->glID(), 0);

	return check(); 
}

void FramebufferData::createColorRenderbuffer(GLenum internalFormat)
{
	glGenRenderbuffers(1, &_colorRenderbuffer);
	checkOpenGLError("glGenRenderbuffers -> color");
	
	glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderbuffer);
	checkOpenGLError("glBindRenderbuffer -> color");
	
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, _size.x, _size.y);
	checkOpenGLError("glRenderbufferStorage -> color");
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderbuffer);
	checkOpenGLError("glFramebufferRenderbuffer -> color");
}

void FramebufferData::createDepthRenderbuffer(GLenum internalFormat)
{
	glGenRenderbuffers(1, &_depthRenderbuffer);
	checkOpenGLError("glGenRenderbuffers -> depth");
	
	glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderbuffer);
	checkOpenGLError("glBindRenderbuffer -> depth");
	
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, _size.x, _size.y);
	checkOpenGLError("glRenderbufferStorage -> depth");
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderbuffer);
	checkOpenGLError("glFramebufferRenderbuffer -> depth");
}

void FramebufferData::forceSize(int w, int h)
{
	_size = vec2i(w, h);
}
