#include <et/device/rendercontext.h>
#include <et/rendering/renderstate.h>
#include <et/vertexbuffer/vertexdeclaration.h>
#include <et/apiobjects/program.h>
#include <et/apiobjects/texture.h>
#include <et/apiobjects/framebuffer.h>
#include <et/apiobjects/vertexarrayobject.h>

using namespace et;

RenderState::RenderState() : 
	_lastTextureUnit(0), _boundFramebuffer(0), _boundArrayBuffer(0),
	_boundElementArrayBuffer(0), _boundVertexArrayObject(0), _boundProgram(0), 
	_blendEnabled(false), _depthTestEnabled(false), _depthMaskEnabled(true), _polygonOffsetFillEnabled(false),
	_wireframe(false), _lastBlend(Blend_Disabled), _lastCull(CullState_None), _lastDepthFunc(DepthFunc_Less)
{
	_boundTextures.fill(0);
	_enabledVertexAttributes.fill(0);
}

void RenderState::setRenderContext(RenderContext* rc)
{
	_rc = rc;
}

void RenderState::setMainViewportSize(const vec2i& sz, bool force)
{
	if (!force && (sz.x == _mainViewportSize.x) && (sz.y == _mainViewportSize.y)) return;

	_mainViewportSize = sz;
	_mainViewportSizeFloat = vec2(static_cast<float>(sz.x), static_cast<float>(sz.y));

	bool shouldSetViewport = (_boundFramebuffer == 0) || (_defaultFramebuffer.valid() && (_boundFramebuffer == _defaultFramebuffer->glID()));
	if (shouldSetViewport)
		glViewport(0, 0, _mainViewportSize.x, _mainViewportSize.y);
}

void RenderState::setViewportSize(const vec2i& sz, bool force)
{
	if (!force && (sz.x == _viewportSize.x) && (sz.y == _viewportSize.y)) return;

	_viewportSize = sz;
	_viewportSizeFloat = vec2(static_cast<float>(sz.x), static_cast<float>(sz.y));
	glViewport(0, 0, _viewportSize.x, _viewportSize.y);
}

void RenderState::bindTexture(GLenum unit, GLuint texture, GLenum target)
{
	if (unit != _lastTextureUnit)
	{
		_lastTextureUnit = unit;
		glActiveTexture(GL_TEXTURE0 + _lastTextureUnit);
	}

	if (_boundTextures[unit] != texture)
	{
		_boundTextures[unit] = texture;
		etBindTexture(target, texture);
	}

}

void RenderState::bindProgram(GLuint program, bool force)
{
	if (force || (program != _boundProgram))
	{ 
		_boundProgram = program;
		etUseProgram(program);
	}
}

void RenderState::bindBuffer(GLenum target, GLuint buffer, bool force)
{ 
	if ((target == GL_ARRAY_BUFFER) && (force || (_boundArrayBuffer != buffer)))
	{
		_boundArrayBuffer = buffer;
		etBindBuffer(target, buffer);
	} 
	else if ((target == GL_ELEMENT_ARRAY_BUFFER) && (force || (_boundElementArrayBuffer != buffer)))
	{ 
		_boundElementArrayBuffer = buffer;
		etBindBuffer(target, buffer);
	} 
	else if ((target != GL_ARRAY_BUFFER) && (target != GL_ELEMENT_ARRAY_BUFFER))
	{
		std::cout << "Trying to bind buffer " << buffer << " to unknown target " << target << std::endl;
	}
}

void RenderState::setVertexAttributes(const VertexDeclaration& decl, bool force)
{
	for (VertexAttributeUsage usage = Usage_Position; usage < Usage_MAX; usage = VertexAttributeUsage(usage + 1))
		setVertexAttribEnabled(usage, decl.has(usage), force);

	setVertexAttributesBaseIndex(decl, 0);
}

void RenderState::setVertexAttributesBaseIndex(const VertexDeclaration& decl, size_t index)
{
	for (size_t i = 0; i < decl.numElements(); ++i) 
	{
		const VertexElement& e = decl.element(i);
		size_t dataOffset = index * (decl.interleaved() ? decl.dataSize() : vertexAttributeTypeSize(e.type) );
		setVertexAttribPointer(e, dataOffset);
	}
}

void RenderState::bindBuffer(const VertexBuffer& buf, bool force)
{
	bindBuffer(GL_ARRAY_BUFFER, buf.valid() ? buf->vertexBuffer() : 0, force);

	if (buf.valid()) 
		setVertexAttributes(buf->declaration(), force);
}

void RenderState::bindBuffer(const IndexBuffer& buf, bool force)
{
	bindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.valid() ? buf->indexBuffer() : 0, force);
}

void RenderState::bindBuffers(const VertexBuffer& vb, const IndexBuffer& ib, bool force)
{
	bindBuffer(vb, force);
	bindBuffer(ib, force);
}

void RenderState::bindVertexArray(GLuint buffer)
{
	if (_boundVertexArrayObject != buffer)
	{ 
		_boundVertexArrayObject = buffer;
		etBindVertexArray(buffer);
	}
}

void RenderState::bindVertexArray(const VertexArrayObject& vao)
{
	bindVertexArray(vao.valid() ? vao->vertexArrayObject() : 0);
}

void RenderState::resetBufferBindings()
{
	bindVertexArray(0);
	bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	bindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderState::bindTexture(GLenum unit, const Texture& texture)
{
	if (texture.valid())
		bindTexture(unit, texture->glID(), texture->target());
	else
		bindTexture(unit, 0, GL_TEXTURE_2D);
}

void RenderState::bindFramebuffer(GLuint framebuffer, GLenum target)
{
	if (_boundFramebuffer != framebuffer)
	{
		_boundFramebuffer = framebuffer;
		etBindFramebuffer(target, framebuffer);
	}
}

void RenderState::bindFramebuffer(const Framebuffer& fbo)
{
	if (fbo.valid())
	{
		bindFramebuffer(fbo->glID());
		setViewportSize(fbo->size());
	}
	else 
	{
		bindFramebuffer(0);
		setViewportSize(_mainViewportSize);
	}
}

void RenderState::bindProgram(const Program& prog, bool force)
{
	if (prog.valid())
		bindProgram(prog->glID(), force);
}

void RenderState::setDefaultFramebuffer(const Framebuffer& framebuffer)
{
	_defaultFramebuffer = framebuffer;
	setMainViewportSize(_defaultFramebuffer->size());
}

void RenderState::bindDefaultFramebuffer(GLenum)
{
	bindFramebuffer(_defaultFramebuffer);
}

void RenderState::setDepthMask(bool enable)
{
	if (_depthMaskEnabled != enable)
	{
		_depthMaskEnabled = enable;
		glDepthMask(enable);
	}
}

void RenderState::setDepthTest(bool enable)
{
	if (enable != _depthTestEnabled)
	{
		_depthTestEnabled = enable;

		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
}

void RenderState::setDepthFunc(DepthFunc func)
{
	if (func == _lastDepthFunc) return;

	_lastDepthFunc = func;
	switch (_lastDepthFunc)
	{
	case DepthFunc_Always:
		{
			glDepthFunc(GL_ALWAYS);
			break;
		}

	case DepthFunc_LessOrEqual:
		{
			glDepthFunc(GL_LEQUAL);
			break;
		}

	case DepthFunc_Equal:
		{
			glDepthFunc(GL_EQUAL);
			break;
		}

	default:
		{
			glDepthFunc(GL_LESS);
			break;
		}
	}
}

void RenderState::setBlend(bool enable, BlendState blend)
{
	if (_blendEnabled != enable)
	{
		_blendEnabled = enable;
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	if ((blend != Blend_Current) && (_lastBlend != blend))
	{
		_lastBlend = blend;
		switch (blend)
		{  
		case Blend_Disabled: 
			{
				glBlendFunc(GL_ONE, GL_ZERO); 
				break;
			}

		case Blend_Default: 
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
				break;
			}

		case Blend_Additive: 
			{
				glBlendFunc(GL_ONE, GL_ONE); 
				break;
			}

		case Blend_AlphaAdditive: 
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
				break;
			}

		case Blend_ColorAdditive: 
			{
				glBlendFunc(GL_SRC_COLOR, GL_ONE); 
				break;
			}

		default: { }
		}
	}
}

void RenderState::vertexArrayDeleted(GLuint buffer)
{
	if (_boundVertexArrayObject == buffer)
		bindVertexArray(0);
}

void RenderState::vertexBufferDeleted(GLuint buffer)
{
	if (_boundArrayBuffer == buffer)
		bindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderState::indexBufferDeleted(GLuint buffer)
{
	if (_boundElementArrayBuffer == buffer)
		bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderState::programDeleted(GLuint program)
{
	if (_boundProgram == program)
		bindProgram(0, true);
}

void RenderState::textureDeleted(GLuint texture)
{
	if (_boundTextures[_lastTextureUnit] == texture)
		bindTexture(_lastTextureUnit, 0, GL_TEXTURE_2D);

	for (GLuint i = 0; i < MAX_TEXTURE_UNITS; ++i) 
	{
		if (_boundTextures[i] == texture)
		{
			_boundTextures[i] = 0;
		}
	}
}

void RenderState::frameBufferDeleted(GLuint buffer)
{
	if (_defaultFramebuffer.valid() && (_defaultFramebuffer->glID() == buffer))
		_defaultFramebuffer = Framebuffer();
	
	if (_boundFramebuffer == buffer)
		bindDefaultFramebuffer();
}

void RenderState::setVertexAttribEnabled(GLuint attrib, bool enabled, bool force)
{
	if (enabled)
	{
		if (!_enabledVertexAttributes[attrib] || force)
		{
			_enabledVertexAttributes[attrib] = enabled;
			glEnableVertexAttribArray(attrib);
			checkOpenGLError("glEnableVertexAttribArray(" + intToStr(attrib) + ")");
		}
	}
	else
	{
		if (_enabledVertexAttributes[attrib] || force)
		{
			_enabledVertexAttributes[attrib] = false;
			glDisableVertexAttribArray(attrib);
			checkOpenGLError("glDisableVertexAttribArray(" + intToStr(attrib) + ")");
		}
	}
}

void RenderState::setVertexAttribPointer(const VertexElement& e, size_t baseIndex)
{
	glVertexAttribPointer(e.usage, vertexAttributeTypeComponents(e.type), vertexAttributeTypeDataType(e.type), 
		false, static_cast<GLsizei>(e.stride), reinterpret_cast<GLvoid*>(e.offset + baseIndex));
	checkOpenGLError("glVertexAttribPointer");
}

void RenderState::setCulling(CullState cull)
{
	if (_lastCull == cull) return;

	switch (cull)
	{
	case CullState_None:
		{
			glDisable(GL_CULL_FACE);
			break;
		}
	case CullState_Back:
		{
			if (_lastCull == CullState_None)
				glEnable(GL_CULL_FACE);

			glCullFace(GL_BACK);
			break;
		}
	case CullState_Front:
		{
			if (_lastCull == CullState_None)
				glEnable(GL_CULL_FACE);

			glCullFace(GL_FRONT);
			break;
		}

	default: 
		return; // error occured
	};

	_lastCull = cull;
}

void RenderState::setPolygonOffsetFill(bool enabled, float factor, float units)
{
	if (_polygonOffsetFillEnabled != enabled)
	{
		_polygonOffsetFillEnabled = enabled;
		if (enabled)
			glEnable(GL_POLYGON_OFFSET_FILL);
		else
			glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glPolygonOffset(factor, units);
}

void RenderState::setWireframeRendering(bool wire)
{
	if (_wireframe != wire)
	{
#if (!ET_OPENGLES)
		_wireframe = wire;
		glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
#endif
	}
}

void RenderState::reset()
{
	setBlend(false, Blend_Disabled);
	setDepthFunc(DepthFunc_Less);
	setDepthMask(true);
	setDepthTest(false);
	setPolygonOffsetFill(false);
	setWireframeRendering(false);
	setCulling(CullState_None);
	bindTexture(0, 0, GL_TEXTURE_2D);
	bindFramebuffer(0);
	bindProgram(0, true);
	setVertexAttributes(VertexDeclaration(), true);
	resetBufferBindings();
/*	
	for (size_t i = 0; i < _boundTextures.size(); ++i)
		_boundTextures[i] = 0;
	
	for (size_t i = 0; i < _enabledVertexAttributes.size(); ++i)
		_enabledVertexAttributes[i] = 0;
*/ 
}