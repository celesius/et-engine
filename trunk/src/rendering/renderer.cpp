/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/opengl/openglcaps.h>
#include <et/rendering/rendercontext.h>
#include <et/rendering/renderer.h>
#include <et/vertexbuffer/indexarray.h>

using namespace et;

extern const std::string fullscreen_vertex_shader; 
extern const std::string fullscreen_scaled_vertex_shader;
extern const std::string scaled_copy_vertex_shader;
extern const std::string copy_fragment_shader;

const size_t defaultTextureUnit = 6;

Renderer::Renderer(RenderContext* rc) : _rc(rc)
{
	checkOpenGLError("Renderer::Renderer");

	IndexArray::Pointer ib(new IndexArray(IndexArrayFormat_8bit, 4, PrimitiveType_TriangleStrips));
	ib->linearize();
	
	VertexArray::Pointer vb(new VertexArray(VertexDeclaration(false, Usage_Position, Type_Vec2), 4));
	RawDataAcessor<vec2> pos = vb->chunk(Usage_Position).accessData<vec2>(0);
	pos[0] = vec2(-1.0, -1.0);
	pos[1] = vec2( 1.0, -1.0);
	pos[2] = vec2(-1.0,  1.0);
	pos[3] = vec2( 1.0,  1.0);

	_fullscreenQuadVao = rc->vertexBufferFactory().createVertexArrayObject("__internal_fullscreen_vao", 
		vb, BufferDrawType_Static, ib, BufferDrawType_Static);

	_fullscreenProgram = rc->programFactory().genProgram(fullscreen_vertex_shader, std::string(), 
		copy_fragment_shader, ProgramDefinesList(), ".", "__fullscreeen__program__");
	_fullscreenProgram->setUniform("color_texture", defaultTextureUnit);

	_fullscreenScaledProgram = rc->programFactory().genProgram(fullscreen_scaled_vertex_shader, std::string(),
		copy_fragment_shader, ProgramDefinesList(), ".", "__fullscreeen_scaled_program__");
	_fullscreenScaledProgram->setUniform("color_texture", defaultTextureUnit);
	_fullScreenScaledProgram_PSUniform = _fullscreenScaledProgram->getUniformLocation("vScale");

	_scaledProgram = rc->programFactory().genProgram(scaled_copy_vertex_shader, std::string(), 
		copy_fragment_shader, ProgramDefinesList(), ".", "__scaled_program__");
	_scaledProgram->setUniform("color_texture", defaultTextureUnit);
	_scaledProgram_PSUniform = _scaledProgram->getUniformLocation("PositionScale");
}

Renderer::~Renderer()
{
}

void Renderer::clear(bool color, bool depth)
{
	GLbitfield mask = (color * GL_COLOR_BUFFER_BIT) + (depth * GL_DEPTH_BUFFER_BIT);

	if (mask)
		glClear(mask);
}

void Renderer::fullscreenPass()
{
	const IndexBuffer& ib = _fullscreenQuadVao->indexBuffer();
	_rc->renderState().bindVertexArray(_fullscreenQuadVao);
	drawElements(ib, 0, ib->size());
}

void Renderer::renderFullscreenTexture(const Texture& texture)
{
	_rc->renderState().bindTexture(defaultTextureUnit, texture);
	_rc->renderState().bindProgram(_fullscreenProgram);
	fullscreenPass();
}

void Renderer::renderFullscreenTexture(const Texture& texture, const vec2& scale)
{
	_rc->renderState().bindTexture(defaultTextureUnit, texture);
	_rc->renderState().bindProgram(_fullscreenScaledProgram);
	_scaledProgram->setUniform(_fullScreenScaledProgram_PSUniform, GL_FLOAT_VEC2, scale);
	fullscreenPass();
}

void Renderer::renderTexture(const Texture& texture, const vec2& position, const vec2& size)
{
	_rc->renderState().bindTexture(defaultTextureUnit, texture);
	_rc->renderState().bindProgram(_scaledProgram);
	_scaledProgram->setUniform(_scaledProgram_PSUniform, GL_FLOAT_VEC4, vec4(position, size));
	fullscreenPass();
}

vec2 Renderer::windowCoordinatesToScene(const vec2i& coord)
{
	const vec2& vpSize = _rc->size();
	return vec2(2.0f * static_cast<float>(coord.x) / vpSize.x - 1.0f, 1.0f - 2.0f * static_cast<float>(coord.y) / vpSize.y );
}

vec2 Renderer::windowSizeToScene(const vec2i& size)
{
	const vec2& vpSize = _rc->size();
	return vec2(2.0f * static_cast<float>(size.x) / vpSize.x, 2.0f * static_cast<float>(size.y) / vpSize.y);
}

void Renderer::renderTexture(const Texture& texture, const vec2i& position, const vec2i& size)
{
	vec2i sz;
	sz.x = (size.x == -1) ? texture->width() : size.x;
	sz.y = (size.y == -1) ? texture->height() : size.y;
	renderTexture(texture, windowCoordinatesToScene(position + vec2i(0, sz.y)), windowSizeToScene(sz));
}

void Renderer::drawElements(const IndexBuffer& ib, size_t first, size_t count)
{
	if (ib.valid())
		etDrawElements(ib->primitiveType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first));
}

void Renderer::drawElements(PrimitiveType primitiveType, const IndexBuffer& ib, size_t first, size_t count)
{
	if (ib.valid())
		etDrawElements(primitiveTypeValue(primitiveType), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first));
}

void Renderer::drawElementsBaseIndex(const VertexArrayObject& vao, int base, size_t first, size_t count)
{
	const IndexBuffer& ib = vao->indexBuffer();
	const VertexBuffer& vb = vao->vertexBuffer();
	if (!ib.valid() || !vb.valid()) return;

#if (ET_OPENGLES)
	RenderState& rs = _rc->renderState();
	rs.bindVertexArray(vao);
	rs.bindBuffer(vb);
	rs.setVertexAttributesBaseIndex(vb->declaration(), base);
	etDrawElements(ib->primitiveType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first));
#else	
	etDrawElementsBaseVertex(ib->geometryType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first), base);
#endif	
}

/*
* Default shaders
*/

const std::string fullscreen_vertex_shader = 
	"etVertexIn vec2 Vertex;"
	"etVertexOut vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex;"
	"gl_Position = vec4(Vertex, 0.0, 1.0);"
	"}";

const std::string fullscreen_scaled_vertex_shader = 
	"uniform vec2 vScale;"
	"etVertexIn vec2 Vertex;"
	"etVertexOut vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex;"
	"gl_Position = vec4(vScale * Vertex, 0.0, 1.0);"
	"}";

const std::string scaled_copy_vertex_shader = 
	"uniform vec4 PositionScale;"
	"etVertexIn vec2 Vertex;"
	"etVertexOut vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex;"
	"gl_Position = vec4(PositionScale.xy + TexCoord * PositionScale.zw, 0.0, 1.0);"
	"}";

const std::string copy_fragment_shader = 
	"uniform sampler2D color_texture;"
	"etFragmentIn etHighp vec2 TexCoord;"
	"void main()"
	"{"
	" etFragmentOut = etTexture2D(color_texture, TexCoord);"
	"}";
