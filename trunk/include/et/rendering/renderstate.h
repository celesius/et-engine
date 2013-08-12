/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/apiobjects/objects.h>

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
		DepthFunc_Never,
		DepthFunc_Less,
		DepthFunc_LessOrEqual,
		DepthFunc_Equal,
		DepthFunc_GreaterOrEqual,
		DepthFunc_Greater,
		DepthFunc_Always
	};

	enum ColorMask
	{
		ColorMask_None = 0x00,
		ColorMask_Red = 0x01,
		ColorMask_Green = 0x02,
		ColorMask_Blue = 0x04,
		ColorMask_Alpha = 0x08,
		ColorMask_RGB = ColorMask_Red | ColorMask_Green | ColorMask_Blue,
		ColorMask_RGBA = ColorMask_RGB | ColorMask_Alpha
	};

	class RenderContext;
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
			StaticDataStorage<uint32_t, MaxTextureUnits> boundTextures;
			StaticDataStorage<size_t, Usage_max> enabledVertexAttributes;
			
			uint32_t activeTextureUnit;
			uint32_t boundFramebuffer; 
			uint32_t boundArrayBuffer;
			uint32_t boundElementArrayBuffer;
			uint32_t boundVertexArrayObject;
			uint32_t boundProgram;
			
			recti clipRect;
			vec2i mainViewportSize;
			vec2i viewportSize;
			vec2 mainViewportSizeFloat;
			vec2 viewportSizeFloat;

			vec4 clearColor;
			size_t colorMask;
			float clearDepth;
			
			float polygonOffsetFactor;
			float polygonOffsetUnits;
			
			bool blendEnabled;
			bool depthTestEnabled;
			bool depthMask;
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
		Framebuffer::Pointer defaultFramebuffer()
			{ return _defaultFramebuffer; }
		
		const Framebuffer::Pointer& defaultFramebuffer() const
			{ return _defaultFramebuffer; }
		
		void setDefaultFramebuffer(const Framebuffer::Pointer& framebuffer);
		
		void bindDefaultFramebuffer();
		void bindFramebuffer(uint32_t framebuffer);
		void bindFramebuffer(const Framebuffer::Pointer& fbo);

		/*
		 * Textures
		 */
		void setActiveTextureUnit(uint32_t unit, bool force = false);
		void bindTexture(uint32_t unit, uint32_t texture, uint32_t target, bool force = false);
		void bindTexture(uint32_t unit, const Texture& texture);

		/*
		 * Programs
		 */
		void bindProgram(uint32_t program, bool force);
		void bindProgram(const Program::Pointer& prog, bool force = false);

		/*
	 	 * Buffers
		 */
		void resetBufferBindings();
		void bindBuffer(uint32_t target, uint32_t buffer, bool force = false);
		void bindVertexArray(uint32_t buffer);

		void bindBuffer(const VertexBuffer& buf, bool force = false);
		void bindBuffer(const IndexBuffer& buf, bool force = false);
		void bindBuffers(const VertexBuffer& vb, const IndexBuffer& ib, bool force = false);
		void bindVertexArray(const VertexArrayObject& vao);

		void setVertexAttributes(const VertexDeclaration& decl, bool force = false);
		void setVertexAttributesBaseIndex(const VertexDeclaration& decl, size_t index);
		void setVertexAttribEnabled(uint32_t attrib, bool enabled, bool foce = false);
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

		bool depthMask() const 
			{ return _currentState.depthMask; }
		
		DepthFunc depthFunc() const
			{ return _currentState.lastDepthFunc; }

		bool wireframeRendering() const 
			{ return _currentState.wireframe; }

		bool clipEnabled() const 
			{ return _currentState.clipEnabled; }

		const recti& clipRect() const 
			{ return _currentState.clipRect; }

		size_t colorMask() const
			{ return _currentState.colorMask; }

		void setBlend(bool enable, BlendState blend = Blend_Current);
		void setCulling(CullState cull);
		void setDepthTest(bool enable);
		void setDepthFunc(DepthFunc func);
		void setDepthMask(bool enable);
		void setPolygonOffsetFill(bool enabled, float factor = 0.0f, float units = 0.0f);
		void setWireframeRendering(bool wire);
		void setClearColor(const vec4& color);
		void setColorMask(size_t mask);
		void setClearDepth(float depth);
		void setClip(bool enable, const recti& clip);

		/*
		 * Deletion handlers
		 */
		void programDeleted(uint32_t program);
		void textureDeleted(uint32_t texture);
		void vertexArrayDeleted(uint32_t buffer);
		void vertexBufferDeleted(uint32_t buffer);
		void indexBufferDeleted(uint32_t buffer);
		void frameBufferDeleted(uint32_t buffer);

		/*
		 * Service
		 */
		static State currentState();
		
	protected:
		void bindDefaultFramebuffer(uint32_t target);
		void bindFramebuffer(uint32_t framebuffer, uint32_t target);

	private:
		friend class RenderContext;
		
		RenderState() :
			_rc(nullptr) { }
		
		RenderState(const RenderState&) :
			_rc(nullptr) { }
		
		RenderState& operator = (const RenderState&)
			{ return *this; }
		
	private:
		RenderContext* _rc;
		Framebuffer::Pointer _defaultFramebuffer;
	
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