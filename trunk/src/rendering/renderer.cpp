#include <et/opengl/openglcaps.h>
#include <et/device/rendercontext.h>
#include <et/rendering/renderer.h>
#include <et/vertexbuffer/IndexArray.h>

using namespace et;

extern const std::string fullscreen_vertex_shader_3_4; 
extern const std::string scaled_copy_vertex_shader_3_4;
extern const std::string copy_fragment_shader_3_4;
extern const std::string fullscreen_vertex_shader_2; 
extern const std::string scaled_copy_vertex_shader_2;
extern const std::string copy_fragment_shader_2;

Renderer::Renderer(RenderContext* rc) : _rc(rc)
{
	checkOpenGLError("Renderer::Renderer");

	IndexArrayRef ib(new IndexArray(IndexArrayFormat_16bit, 4, IndexArrayContentType_TriangleStrips));
	ib->linearize();
	
	VertexArrayRef vb(new VertexArray(VertexDeclaration(false, Usage_Position, Type_Vec2), 4));
	RawDataAcessor<vec2> pos = vb->chunk(Usage_Position).accessData<vec2>(0);
	pos[0] = vec2(-1.0, -1.0);
	pos[1] = vec2( 1.0, -1.0);
	pos[2] = vec2(-1.0,  1.0);
	pos[3] = vec2( 1.0,  1.0);

	_fullscreenQuadVao = rc->vertexBufferFactory().createVertexArrayObject("fsquad-vao");
	_fullscreenQuadVao->setBuffers(
		rc->vertexBufferFactory().createVertexBuffer("fsquad-vert", vb, BufferDrawType_Static),
		rc->vertexBufferFactory().createIndexBuffer("fsquad-ind", ib, BufferDrawType_Static));

	std::string fragSource = (ogl_caps().version() == OpenGLVersion_Old) ? copy_fragment_shader_2 : copy_fragment_shader_3_4;
	std::string fsVertSource = (ogl_caps().version() == OpenGLVersion_Old) ? fullscreen_vertex_shader_2 : fullscreen_vertex_shader_3_4;
	std::string scaledVertSource = (ogl_caps().version() == OpenGLVersion_Old) ? scaled_copy_vertex_shader_2 : scaled_copy_vertex_shader_3_4;

	_fullscreenProgram = rc->programFactory().genProgram(fsVertSource, "", fragSource, ProgramDefinesList(), ".", "__fullscreeen__program__");
	_fullscreenProgram->setUniform("color_texture", 0);

	_scaledProgram = rc->programFactory().genProgram(scaledVertSource, "", fragSource, ProgramDefinesList(), ".", "__scaled__program__");
	_scaledProgram->setUniform("color_texture", 0);
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

void Renderer::setClearColor(const vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::fullscreenPass()
{
	const IndexBuffer& ib = _fullscreenQuadVao->indexBuffer();
	_rc->renderState().bindVertexArray(_fullscreenQuadVao);
	drawElements(ib, 0, ib->size());
}

void Renderer::renderFullscreenTexture(const Texture& texture)
{
	_rc->renderState().bindTexture(0, texture);
	_rc->renderState().bindProgram(_fullscreenProgram);
	_fullscreenProgram->setUniform("cColor", vec4(1.0f));
	fullscreenPass();
}

void Renderer::renderTexture(const Texture& texture, const vec2& position, const vec2& size)
{
	_rc->renderState().bindTexture(0, texture);
	_rc->renderState().bindProgram(_scaledProgram);
	_scaledProgram->setUniform(_scaledProgram_PSUniform, GL_FLOAT_VEC4, vec4(position.x, position.y - size.y, size.x, size.y) );
	_scaledProgram->setUniform("cColor", vec4(1.0f));
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
	renderTexture(texture, windowCoordinatesToScene(position), windowSizeToScene(sz));
}

void Renderer::drawElements(const IndexBuffer& ib, size_t first, size_t count)
{
	if (ib.valid())
		etDrawElements(ib->geometryType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first));
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
	etDrawElements(ib->geometryType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first));
#else	
	etDrawElementsBaseVertex(ib->geometryType(), static_cast<GLsizei>(count), ib->dataType(), ib->indexOffset(first), base);
#endif	
}

/*
* Default shaders
*/

const std::string fullscreen_vertex_shader_3_4 = 
	"in vec2 Vertex;"
	"out vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex.xy;"
	"gl_Position = vec4(Vertex.xy, 0.0, 1.0);"
	"}";

const std::string scaled_copy_vertex_shader_3_4 = 
	"uniform highp vec4 PositionScale;"
	"in vec2 Vertex;"
	"out vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex.xy;"
	"gl_Position = vec4(PositionScale.xy + TexCoord * PositionScale.zw, 0.0, 1.0);"
	"}";

const std::string copy_fragment_shader_3_4 = 
	"uniform sampler2D color_texture;"
	"uniform highp vec4 cColor;"
	"in vec2 TexCoord;"
	"out vec4 FragColor;"
	"void main()"
	"{"
	" FragColor = texture(color_texture, TexCoord) * cColor;"
	"}";

/*
* OpenGL 2 (OpenGL ES) shaders
*/

const std::string fullscreen_vertex_shader_2 = 
	"attribute "ET_HIGH_PRECISION" vec2 Vertex;"
	"varying "ET_HIGH_PRECISION" vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex.xy;"
	"gl_Position = vec4(Vertex.xy, 0.0, 1.0);"
	"}";

const std::string scaled_copy_vertex_shader_2 = 
	"uniform "ET_HIGH_PRECISION" vec4 PositionScale;"
	"attribute "ET_HIGH_PRECISION" vec2 Vertex;"
	"varying "ET_HIGH_PRECISION" vec2 TexCoord;"
	"void main()"
	"{"
	"TexCoord = vec2(0.5) + 0.5 * Vertex.xy;"
	"gl_Position = vec4(PositionScale.xy + TexCoord * PositionScale.zw, 0.0, 1.0);"
	"}";

const std::string copy_fragment_shader_2 = 
	"uniform sampler2D color_texture;"
	"uniform "ET_LOW_PRECISION" vec4 cColor;"
	"varying "ET_HIGH_PRECISION" vec2 TexCoord;"
	"void main()"
	"{"
	" gl_FragColor = texture2D(color_texture, TexCoord) * cColor;"
	"}";