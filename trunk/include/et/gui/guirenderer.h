/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <stack>
#include <et/core/containers.h>
#include <et/camera/camera.h>
#include <et/gui/font.h>
#include <et/gui/renderingelement.h>

namespace et
{
	namespace gui
	{
		class GuiRenderer
		{
		public:
			GuiRenderer(RenderContext* rc, bool saveFillRate);

			void beginRender(RenderContext* rc);
			void render(RenderContext* rc);
			void endRender(RenderContext* rc);

			void resetClipRect();
			void pushClipRect(const recti&);
			void popClipRect();

			void setProjectionMatrices(const vec2& contextSize);
			void setRendernigElement(const RenderingElement::Pointer& r);

			size_t addVertices(const GuiVertexList& vertices, const Texture& texture, ElementClass cls, GuiRenderLayer layer);
			
			int measusevertexCountForImageDescriptor(const ImageDescriptor& desc);
			
			void createStringVertices(GuiVertexList& vertices, const CharDescriptorList& chars, 
									  ElementAlignment hAlign, ElementAlignment vAlign, const vec2& pos, 
									  const vec4& color, const mat4& transform, GuiRenderLayer layer);
			
			void createImageVertices(GuiVertexList& vertices, const Texture& tex, const ImageDescriptor& desc, 
									 const rect& p, const vec4& color, const mat4& transform, GuiRenderLayer layer);
			
			void createColorVertices(GuiVertexList& vertices, const rect& p, const vec4& color, const mat4& transform, 
									 GuiRenderLayer layer);
			
			void buildQuad(GuiVertexList& vertices, const GuiVertex& topLeft, const GuiVertex& topRight,
						   const GuiVertex& bottomLeft, const GuiVertex& bottomRight);
			
			void setCustomOffset(const vec2& offset)
				{ _customOffset = 2.0f * offset; }
			
			void setCustomAlpha(float alpha)
				{ _customAlpha = alpha; }
			
			const Camera& camera3d() const 
				{ return _guiCamera; }

		private:
			void init(RenderContext* rc);
			void alloc(size_t count);
			GuiVertexPointer allocateVertices(size_t count, const Texture& texture, ElementClass cls, GuiRenderLayer layer);
			
			GuiRenderer& operator = (const GuiRenderer&)
				{ return *this; }
			
		private:
			RenderContext* _rc;
			RenderingElement::Pointer _renderingElement;
			Texture _lastTextures[GuiRenderLayer_max];
			
			Program _guiProgram;
			mat4 _defaultTransform;
			GLint _guiDefaultTransformUniform;
			GLint _guiCustomOffsetUniform;
			GLint _guiCustomAlphaUniform;
			
			Camera _guiCamera;

			std::stack<recti> _clip;
			vec2 _customOffset;
			float _customAlpha;
	
			bool _saveFillRate;
			bool _depthTestEnabled;
			bool _depthMaskEnabled;
			bool _blendEnabled;
			BlendState _blendState;
		};
		
	}
}
