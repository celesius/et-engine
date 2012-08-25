/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/rendering/rendercontext.h>
#include <et/rendering/renderstate.h>
#include <et/vertexbuffer/vertexdeclaration.h>
#include <et/apiobjects/program.h>
#include <et/apiobjects/texture.h>
#include <et/apiobjects/framebuffer.h>
#include <et/apiobjects/vertexarrayobject.h>

using namespace et;

static const std::string keyCurrentStateBegin = "RenderState::currentState() - begin";
static const std::string keyCurrentStateEnd = "RenderState::currentState() - end";
static const std::string keyEnableVertexAttribArray = "glEnableVertexAttribArray(...)";
static const std::string keyDisableVertexAttribArray = "glDisableVertexAttribArray(...)";
static const std::string keyVertexAttribPointer = "glVertexAttribPointer(...)";

RenderState::State::State() : 
	activeTextureUnit(0), boundFramebuffer(0), boundArrayBuffer(0), boundElementArrayBuffer(0), boundVertexArrayObject(0), 
	boundProgram(0), polygonOffsetFactor(0.0f), polygonOffsetUnits(0.0f), blendEnabled(false), depthTestEnabled(false),
	depthMaskEnabled(true), polygonOffsetFillEnabled(false), wireframe(false), lastBlend(Blend_Disabled), 
	lastCull(CullState_None), lastDepthFunc(DepthFunc_Less)
{
	boundTextures.fill(0);
	enabledVertexAttributes.fill(0);
}

PreservedRenderStateScope::PreservedRenderStateScope(RenderState& rs, bool shouldApplyBefore) : 
	_rs(rs), _state(RenderState::currentState())
{
	if (shouldApplyBefore)
		_rs.applyState(_state);
}

PreservedRenderStateScope::~PreservedRenderStateScope()
{
	_rs.applyState(_state);
}

void RenderState::setRenderContext(RenderContext* rc)
{
	_rc = rc;
	_currentState = RenderState::currentState();
}

void RenderState::setMainViewportSize(const vec2i& sz, bool force)
{
	if (!force && (sz.x == _currentState.mainViewportSize.x) && (sz.y == _currentState.mainViewportSize.y)) return;

	_currentState.mainViewportSize = sz;
	_currentState.mainViewportSizeFloat = vec2(static_cast<float>(sz.x), static_cast<float>(sz.y));

	bool shouldSetViewport = (_currentState.boundFramebuffer == 0) ||
		(_defaultFramebuffer.valid() && (_currentState.boundFramebuffer == _defaultFramebuffer->glID()));
	
	if (shouldSetViewport)
		etViewport(0, 0, _currentState.mainViewportSize.x, _currentState.mainViewportSize.y);
}

void RenderState::setViewportSize(const vec2i& sz, bool force)
{
	if (!force && (sz.x == _currentState.viewportSize.x) && (sz.y == _currentState.viewportSize.y)) return;

	_currentState.viewportSize = sz;
	_currentState.viewportSizeFloat = vec2(static_cast<float>(sz.x), static_cast<float>(sz.y));
	etViewport(0, 0, _currentState.viewportSize.x, _currentState.viewportSize.y);
}

void RenderState::setActiveTextureUnit(GLenum unit, bool force)
{
	if ((unit != _currentState.activeTextureUnit) || force)
	{
		_currentState.activeTextureUnit = unit;
		glActiveTexture(GL_TEXTURE0 + _currentState.activeTextureUnit);
	}
}

void RenderState::bindTexture(GLenum unit, GLuint texture, GLenum target)
{
	setActiveTextureUnit(unit, false);
	if (_currentState.boundTextures[unit] != texture)
	{
		_currentState.boundTextures[unit] = texture;
		etBindTexture(target, texture);
	}

}

void RenderState::bindProgram(GLuint program, bool force)
{
	if (force || (program != _currentState.boundProgram))
	{ 
		_currentState.boundProgram = program;
		etUseProgram(program);
	}
}

void RenderState::bindBuffer(GLenum target, GLuint buffer, bool force)
{ 
	if ((target == GL_ARRAY_BUFFER) && (force || (_currentState.boundArrayBuffer != buffer)))
	{
		_currentState.boundArrayBuffer = buffer;
		etBindBuffer(target, buffer);
	} 
	else if ((target == GL_ELEMENT_ARRAY_BUFFER) && (force || (_currentState.boundElementArrayBuffer != buffer)))
	{ 
		_currentState.boundElementArrayBuffer = buffer;
		etBindBuffer(target, buffer);
	} 
	else if ((target != GL_ARRAY_BUFFER) && (target != GL_ELEMENT_ARRAY_BUFFER))
	{
		std::cout << "Trying to bind buffer " << buffer << " to unknown target " << target << std::endl;
	}
}

void RenderState::setVertexAttributes(const VertexDeclaration& decl, bool force)
{
	for (VertexAttributeUsage usage = Usage_Position; usage < Usage_max; usage = VertexAttributeUsage(usage + 1))
		setVertexAttribEnabled(usage, decl.has(usage), force);

	setVertexAttributesBaseIndex(decl, 0);
}

void RenderState::setVertexAttributesBaseIndex(const VertexDeclaration& decl, size_t index)
{
	for (size_t i = 0; i < decl.numElements(); ++i) 
	{
		const VertexElement& e = decl.element(i);
		size_t dataOffset = index * (decl.interleaved() ? decl.dataSize() : vertexAttributeTypeSize(e.type()) );
		setVertexAttribPointer(e, dataOffset);
	}
}

void RenderState::bindBuffer(const VertexBuffer& buf, bool force)
{
	bindBuffer(GL_ARRAY_BUFFER, buf.valid() ? buf->glID() : 0, force);

	if (buf.valid()) 
		setVertexAttributes(buf->declaration(), force);
}

void RenderState::bindBuffer(const IndexBuffer& buf, bool force)
{
	bindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.valid() ? buf->glID() : 0, force);
}

void RenderState::bindBuffers(const VertexBuffer& vb, const IndexBuffer& ib, bool force)
{
	bindBuffer(vb, force);
	bindBuffer(ib, force);
}

void RenderState::bindVertexArray(GLuint buffer)
{
	if (_currentState.boundVertexArrayObject != buffer)
	{ 
		_currentState.boundVertexArrayObject = buffer;
		etBindVertexArray(buffer);
	}
}

void RenderState::bindVertexArray(const VertexArrayObject& vao)
{
	if (vao.valid())
	{
		bindVertexArray(vao->glID());
		bindBuffers(vao->vertexBuffer(), vao->indexBuffer());
	}
	else
	{
		bindVertexArray(0);
	}
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
	if (_currentState.boundFramebuffer != framebuffer)
	{
		_currentState.boundFramebuffer = framebuffer;
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
		setViewportSize(_currentState.mainViewportSize);
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
	if (_currentState.depthMaskEnabled != enable)
	{
		_currentState.depthMaskEnabled = enable;
		glDepthMask(enable);
	}
}

void RenderState::setDepthTest(bool enable)
{
	if (enable != _currentState.depthTestEnabled)
	{
		_currentState.depthTestEnabled = enable;

		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
}

void RenderState::setDepthFunc(DepthFunc func)
{
	if (func == _currentState.lastDepthFunc) return;

	_currentState.lastDepthFunc = func;
	switch (_currentState.lastDepthFunc)
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
	if (_currentState.blendEnabled != enable)
	{
		_currentState.blendEnabled = enable;
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	if ((blend != Blend_Current) && (_currentState.lastBlend != blend))
	{
		_currentState.lastBlend = blend;
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
				
		case Blend_AlphaPremultiplied:
			{
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				break;
			}

		case Blend_ColorAdditive: 
			{
				glBlendFunc(GL_SRC_COLOR, GL_ONE); 
				break;
			}

		default:
			break;
		}
	}
}

void RenderState::vertexArrayDeleted(GLuint buffer)
{
	if (_currentState.boundVertexArrayObject == buffer)
		bindVertexArray(0);
}

void RenderState::vertexBufferDeleted(GLuint buffer)
{
	if (_currentState.boundArrayBuffer == buffer)
		bindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderState::indexBufferDeleted(GLuint buffer)
{
	if (_currentState.boundElementArrayBuffer == buffer)
		bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderState::programDeleted(GLuint program)
{
	if (_currentState.boundProgram == program)
		bindProgram(0, true);
}

void RenderState::textureDeleted(GLuint texture)
{
	if (_currentState.boundTextures[_currentState.activeTextureUnit] == texture)
		bindTexture(_currentState.activeTextureUnit, 0, GL_TEXTURE_2D);

	for (GLuint i = 0; i < MAX_TEXTURE_UNITS; ++i) 
	{
		if (_currentState.boundTextures[i] == texture)
			_currentState.boundTextures[i] = 0;
	}
}

void RenderState::frameBufferDeleted(GLuint buffer)
{
	if (_defaultFramebuffer.valid() && (_defaultFramebuffer->glID() == buffer))
		_defaultFramebuffer = Framebuffer();
	
	if (_currentState.boundFramebuffer == buffer)
		bindDefaultFramebuffer();
}

void RenderState::setVertexAttribEnabled(GLuint attrib, bool enabled, bool force)
{
	bool wasEnabled = _currentState.enabledVertexAttributes[attrib];

	if (enabled && (!wasEnabled || force))
	{
		glEnableVertexAttribArray(attrib);
		checkOpenGLError(keyEnableVertexAttribArray);
	}
	else if (!enabled && (wasEnabled || force))
	{
		glDisableVertexAttribArray(attrib);
		checkOpenGLError(keyDisableVertexAttribArray);
	}
	
	_currentState.enabledVertexAttributes[attrib] = enabled;
}

void RenderState::setVertexAttribPointer(const VertexElement& e, size_t baseIndex)
{
	glVertexAttribPointer(e.usage(), e.components(), e.dataType(), false, e.stride(), 
		reinterpret_cast<GLvoid*>(e.offset() + baseIndex));

	checkOpenGLError(keyVertexAttribPointer);
}

void RenderState::setCulling(CullState cull)
{
	if (_currentState.lastCull == cull) return;

	switch (cull)
	{
	case CullState_None:
		{
			glDisable(GL_CULL_FACE);
			break;
		}
	case CullState_Back:
		{
			if (_currentState.lastCull == CullState_None)
				glEnable(GL_CULL_FACE);

			glCullFace(GL_BACK);
			break;
		}
	case CullState_Front:
		{
			if (_currentState.lastCull == CullState_None)
				glEnable(GL_CULL_FACE);

			glCullFace(GL_FRONT);
			break;
		}

	default: 
		return; // error occured
	};

	_currentState.lastCull = cull;
}

void RenderState::setPolygonOffsetFill(bool enabled, float factor, float units)
{
	if (_currentState.polygonOffsetFillEnabled != enabled)
	{
		_currentState.polygonOffsetFillEnabled = enabled;
		if (enabled)
			glEnable(GL_POLYGON_OFFSET_FILL);
		else
			glDisable(GL_POLYGON_OFFSET_FILL);
	}

	_currentState.polygonOffsetFactor = factor;
	_currentState.polygonOffsetUnits = units;
	glPolygonOffset(factor, units);
}

void RenderState::setWireframeRendering(bool wire)
{
	if (_currentState.wireframe != wire)
	{
#if (!ET_OPENGLES)
		_currentState.wireframe = wire;
		glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
#endif
	}
}

void RenderState::setClearColor(const vec4& color)
{
	_currentState.clearColor = color;
	glClearColor(color.x, color.y, color.z, color.w);
}

void RenderState::reset()
{
	applyState(RenderState::State());
}

void RenderState::applyState(const RenderState::State& s)
{
	setBlend(s.blendEnabled, s.lastBlend);
	setDepthFunc(s.lastDepthFunc);
	setDepthMask(s.depthMaskEnabled);
	setDepthTest(s.depthTestEnabled);
	setPolygonOffsetFill(s.polygonOffsetFillEnabled, s.polygonOffsetFactor, s.polygonOffsetUnits);
	setWireframeRendering(s.wireframe);
	setCulling(s.lastCull);
	setViewportSize(s.viewportSize);
	bindFramebuffer(s.boundFramebuffer);
	bindProgram(s.boundProgram, true);
	bindVertexArray(s.boundVertexArrayObject);
	bindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.boundElementArrayBuffer, true);
	bindBuffer(GL_ARRAY_BUFFER, s.boundArrayBuffer, true);
	
	for (size_t i = 0; i < MAX_TEXTURE_UNITS; ++i)
		bindTexture(i, s.boundTextures[i], GL_TEXTURE_2D);
	
	for (size_t i = 0; i < Usage_max; ++i)
		setVertexAttribEnabled(i, s.enabledVertexAttributes[i], false);
	
	setActiveTextureUnit(s.activeTextureUnit);
}

RenderState::State RenderState::currentState()
{
	checkOpenGLError(keyCurrentStateBegin);
	State s;

	GLint value = 0;
	for (size_t i = 0; i < Usage_max; ++i)
	{
		int enabled = 0;
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		s.enabledVertexAttributes[i] = (enabled > 0);
	}

	value = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &value);
	s.activeTextureUnit = value - GL_TEXTURE0;
	
	for (size_t i = 0; i < MAX_TEXTURE_UNITS; ++i)
	{
		value = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &value);
		s.boundTextures[i] = value;
	}
	glActiveTexture(GL_TEXTURE0 + s.activeTextureUnit);

	value = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &value);
	s.boundArrayBuffer = value;

	value = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &value);
	s.boundElementArrayBuffer = value;

	value = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &value);
	s.boundFramebuffer = value;

	value = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &value);
	s.boundVertexArrayObject = value;

	value = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &value);
	s.boundProgram = value;

	s.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST) != 0;
	s.polygonOffsetFillEnabled = glIsEnabled(GL_POLYGON_OFFSET_FILL) != 0;
	
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &s.polygonOffsetFactor);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &s.polygonOffsetUnits);
	
	unsigned char bValue = 0;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &bValue);
	s.depthMaskEnabled = bValue > 0;

	value = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &value);
	s.lastCull = CullState_None;
	if (value == GL_FRONT)
		s.lastCull = CullState_Front;
	else if (value == GL_BACK)
		s.lastCull = CullState_Back;

	bValue = 0;
	glGetBooleanv(GL_CULL_FACE, &bValue);
	if (!bValue)
		s.lastCull = CullState_None;

	vec4i vp;
	glGetIntegerv(GL_VIEWPORT, vp.data());
	s.viewportSize = vec2i(vp.z, vp.w);
	s.viewportSizeFloat = vec2(static_cast<float>(s.viewportSize.x), static_cast<float>(s.viewportSize.y));
	s.mainViewportSize = s.viewportSize;
	s.mainViewportSizeFloat = s.viewportSizeFloat;
	
	value = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &value);
	s.lastDepthFunc = DepthFunc_Less;
	if (value == GL_LEQUAL)
		s.lastDepthFunc = DepthFunc_LessOrEqual;
	else if (value == GL_EQUAL)
		s.lastDepthFunc = DepthFunc_Equal;
	else if (value == GL_ALWAYS)
		s.lastDepthFunc = DepthFunc_Always;

	glGetFloatv(GL_COLOR_CLEAR_VALUE, s.clearColor.data());

	// TODO: get this from state, too lazy now.
	s.wireframe = false;
	
	value = 0;
	glGetIntegerv(GL_BLEND, &value);
	s.blendEnabled = value != 0;
	
	int blendDest = 0;
	glGetIntegerv(GL_BLEND_DST_RGB, &blendDest);
	
	int blendSource = 0;
	glGetIntegerv(GL_BLEND_SRC_RGB, &blendSource);

	if ((blendSource == GL_SRC_ALPHA) && (blendDest == GL_ONE_MINUS_SRC_ALPHA))
	{
		s.lastBlend = Blend_Default;
	}
	else if ((blendSource == GL_ONE) && (blendDest == GL_ONE_MINUS_SRC_ALPHA))
	{
		s.lastBlend = Blend_AlphaPremultiplied;
	}
	else if ((blendSource == GL_ONE) && (blendDest == GL_ZERO))
	{
		s.lastBlend = Blend_Disabled;
	}
	else if ((blendSource == GL_ONE) && (blendDest == GL_ONE))
	{
		s.lastBlend = Blend_Additive;
	}
	else if ((blendSource == GL_SRC_ALPHA) && (blendDest == GL_ONE))
	{
		s.lastBlend = Blend_AlphaAdditive;
	}
	else if ((blendSource == GL_SRC_COLOR) && (blendDest == GL_ONE))
	{
		s.lastBlend = Blend_ColorAdditive;
	}
	else
	{
		std::cout << "Unsupported blend combination: " << glBlendFuncToString(blendSource) <<
						", " << glBlendFuncToString(blendDest) << std::endl;
		
		assert("Unsupported blend combination" && 0);
	}

	checkOpenGLError(keyCurrentStateEnd);
	
	return s;
}