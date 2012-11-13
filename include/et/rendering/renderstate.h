/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/rendering/rendering.h>
#include <et/apiobjects/framebuffer.h>

namespace et
{
	enum BlendState
	{
		Blend_Disabled,
		Blend_Current,
		Blend_Default,
		Blend_AlphaPremultiplied,
		Blend_Additive,
		Blend_AlphaAdditive,
		Blend_ColorAdditive
	};

	enum CullState
	{
		CullState_None,
		CullState_Front,
		CullState_Back
	};

	enum DepthFunc
	{
		DepthFunc_Less,
		DepthFunc_LessOrEqual,
		DepthFunc_Equal,
		DepthFunc_Always
	};

	class RenderContext;
	class Texture;
	class Program;
	class Framebuffer;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArrayObject;
	class VertexDeclaration;
	class VertexElement;

	class RenderState
	{
	public:
		struct State
		{
			GLuint boundTextures[MAX_TEXTURE_UNITS]; // StaticDataStorage<GLuint, MAX_TEXTURE_UNITS> boundTextures;
			bool enabledVertexAttributes[Usage_max]; // StaticDataStorage<bool, Usage_max> enabledVertexAttributes;
			
			GLenum activeTextureUnit;
			GLenum boundFramebuffer; 
			GLenum boundArrayBuffer;
			GLenum boundElementArrayBuffer;
			GLuint boundVertexArrayObject;
			GLuint boundProgram;
			
			recti clipRect;
			vec2i mainViewportSize;
			vec2i viewportSize;
			vec2 mainViewportSizeFloat;
			vec2 viewportSizeFloat;

			vec4 clearColor;
			
			float polygonOffsetFactor;
			float polygonOffsetUnits;
			
			bool blendEnabled;
			bool depthTestEnabled;
			bool depthMaskEnabled;
			bool polygonOffsetFillEnabled;
			bool wireframe;
			bool clipEnabled;

			BlendState lastBlend;
			CullState lastCull;
			DepthFunc lastDepthFunc;		
			
			State();
		};

	public: 
		void setRenderContext(RenderContext* rc);
		void reset();
		void applyState(const State& s);

		/*
		 * Viewport
		 */
		const vec2i& mainViewportSize() const
			{ return _currentState.mainViewportSize; }
		const vec2i& viewportSize() const
			{ return _currentState.viewportSize; }
		const vec2& mainViewportSizeFloat() const
			{ return _currentState.mainViewportSizeFloat; }
		const vec2& viewportSizeFloat() const
			{ return _currentState.viewportSizeFloat; }

		void setMainViewportSize(const vec2i& sz, bool force = false);
		void setViewportSize(const vec2i& sz, bool force = false);

		/*
		 * Framebuffers
		 */
		void setDefaultFramebuffer(const Framebuffer& framebuffer);
		void bindDefaultFramebuffer(GLenum target = GL_FRAMEBUFFER);
		void bindFramebuffer(GLuint framebuffer, GLenum target = GL_FRAMEBUFFER);
		void bindFramebuffer(const Framebuffer& fbo);

		/*
		 * Textures
		 */
		void setActiveTextureUnit(GLenum unit, bool force = false);
		void bindTexture(GLenum unit, GLuint texture, GLenum target);
		void bindTexture(GLenum unit, const Texture& texture);

		/*
		 * Programs
		 */
		void bindProgram(GLuint program, bool force);
		void bindProgram(const Program& prog, bool force = false);

		/*
	 	 * Buffers
		 */
		void resetBufferBindings();
		void bindBuffer(GLenum target, GLuint buffer, bool force = false);
		void bindVertexArray(GLuint buffer);

		void bindBuffer(const VertexBuffer& buf, bool force = false);
		void bindBuffer(const IndexBuffer& buf, bool force = false);
		void bindBuffers(const VertexBuffer& vb, const IndexBuffer& ib, bool force = false);
		void bindVertexArray(const VertexArrayObject& vao);

		void setVertexAttributes(const VertexDeclaration& decl, bool force = false);
		void setVertexAttributesBaseIndex(const VertexDeclaration& decl, size_t index);
		void setVertexAttribEnabled(GLuint attrib, bool enabled, bool foce = false);
		void setVertexAttribPointer(const VertexElement& e, size_t baseIndex);

		/*
		 * State
		 */
		bool blendEnabled() const
			{ return _currentState.blendEnabled; }

		BlendState blendState() const
			{ return _currentState.lastBlend; }

		bool depthTestEnabled() const 
			{ return _currentState.depthTestEnabled; }

		bool depthMaskEnabled() const 
			{ return _currentState.depthMaskEnabled; }
		
		DepthFunc depthFunc() const
			{ return _currentState.lastDepthFunc; }

		bool wireframeRendering() const 
			{ return _currentState.wireframe; }

		bool clipEnabled() const 
			{ return _currentState.clipEnabled; }

		const recti& clipRect() const 
			{ return _currentState.clipRect; }

		void setBlend(bool enable, BlendState blend = Blend_Current);
		void setCulling(CullState cull);
		void setDepthTest(bool enable);
		void setDepthFunc(DepthFunc func);
		void setDepthMask(bool enable);
		void setPolygonOffsetFill(bool enabled, float factor = 0.0f, float units = 0.0f);
		void setWireframeRendering(bool wire);
		void setClearColor(const vec4& color);
		void setClip(bool enable, const recti& clip);

		/*
		 * Deletion handlers
		 */
		void programDeleted(GLuint program);
		void textureDeleted(GLuint texture);
		void vertexArrayDeleted(GLuint buffer);
		void vertexBufferDeleted(GLuint buffer);
		void indexBufferDeleted(GLuint buffer);
		void frameBufferDeleted(GLuint buffer);

		/*
		 * Service
		 */
		static State currentState();

	private:
		friend class RenderContext;
		
		RenderState() : _rc(0) { }
		RenderState(const RenderState&) : _rc(0) { }
		RenderState& operator = (const RenderState&) { return *this; }
		
	private:
		RenderContext* _rc;
		Framebuffer _defaultFramebuffer;
	
		State _currentState;
	};
	
	class PreservedRenderStateScope
	{
	public:
		PreservedRenderStateScope(RenderState& rs, bool shouldApplyBefore);
		~PreservedRenderStateScope();

	private:
		PreservedRenderStateScope& operator = (const PreservedRenderStateScope&)
			{ return *this; }
		
	private:
		RenderState& _rs;
		RenderState::State _state;
	};

}