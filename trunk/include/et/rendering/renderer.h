#pragma once

#include <et/core/autoptr.h>
#include <et/rendering/renderstate.h>
#include <et/apiobjects/vertexarrayobject.h>
#include <et/apiobjects/texture.h>
#include <et/apiobjects/program.h>

namespace et
{
	class RenderContext;
	class Renderer
	{
	public: 
		Renderer(RenderContext*);
		~Renderer();

		void clear(bool color = true, bool depth = true);
		void setClearColor(const vec4& color);

		void fullscreenPass();
		void renderFullscreenTexture(const Texture&);
		void renderTexture(const Texture&, const vec2& position, const vec2& size);
		void renderTexture(const Texture&, const vec2i& position, const vec2i& size = vec2i(-1));

		void drawElements(const IndexBuffer& ib, size_t first, size_t count);
		void drawElementsBaseIndex(const VertexArrayObject& vao, int base, size_t first, size_t count);

		vec2 windowCoordinatesToScene(const vec2i& coord);
		vec2 windowSizeToScene(const vec2i& size);

	private:
		Renderer& operator = (const Renderer&) 
			{ return *this; }

	private:
		RenderContext* _rc;
		VertexArrayObject _fullscreenQuadVao;

		Program _fullscreenProgram;
		Program _scaledProgram;
		int _scaledProgram_PSUniform;
	};
}