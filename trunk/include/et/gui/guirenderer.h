#pragma once

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
			GuiRenderer(RenderContext* rc);
			void setProjectionMatrices(const vec2& contextSize);

			void setRendernigElement(const RenderingElementRef& r);

			void beginRender(RenderContext* rc);
			void render(RenderContext* rc);
			void endRender(RenderContext* rc);

			inline const Camera& camera3d() const 
				{ return _guiCamera; }

			size_t addVertices(const GuiVertexList& vertices, const Texture& texture, 
				ElementClass cls, GuiRenderLayer layer);

			int measuseVerticesCountForImageDescriptor(const ImageDescriptor& desc);

			void createStringVertices(GuiVertexList& vertices, const CharDescriptorList& chars, const vec2& pos, 
				const vec4& color, const mat4& transform, GuiRenderLayer layer);

			void createImageVertices(GuiVertexList& vertices, const Texture& tex, const ImageDescriptor& desc, 
				const rect& p, const vec4& color, const mat4& transform, GuiRenderLayer layer);

			void buildQuad(GuiVertexList& vertices, const GuiVertex& topLeft, const GuiVertex& topRight,
							const GuiVertex& bottomLeft, const GuiVertex& bottomRight);
			
			inline void setCustomOffset(const vec2& offset)
				{ _customOffset = 2.0f * offset; }

			inline void setCustomAlpha(float alpha)
				{ _customAlpha = alpha; }

		private:
			void init(RenderContext* rc);
			void alloc(size_t count);
			GuiVertexPointer allocateVertices(size_t count, const Texture& texture, ElementClass cls, GuiRenderLayer layer);

			GuiRenderer& operator = (const GuiRenderer&)
				{ return *this; }

		private:
			RenderingElementRef _renderingElement;
			Texture _lastTextures[GuiRenderLayer_max];

			Program _guiProgram;
			mat4 _defaultTransform;
			GLint _guiDefaultTransformUniform;
			GLint _guiCustomOffsetUniform;
			GLint _guiCustomAlphaUniform;

			Camera _guiCamera;

			vec2 _customOffset;
			float _customAlpha;

			bool _depthTestEnabled;
			bool _depthMaskEnabled;
			bool _blendEnabled;
			BlendState _blendState;
		};

	}
}