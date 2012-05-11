/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/guirenderer.h>
#include <et/opengl/openglcaps.h>

using namespace et;
using namespace et::gui;

const size_t BlockSize = 1024;

extern std::string gui_vertex_src;
extern std::string gui_frag_src;

GuiRenderer::GuiRenderer(RenderContext* rc) : _customAlpha(1.0f)
{
	_guiProgram = rc->programFactory().genProgram(gui_vertex_src, std::string(), gui_frag_src,
		ProgramDefinesList(),  std::string(), "shader-gui");

	_guiDefaultTransformUniform = _guiProgram->getUniformLocation("mDefaultTransform");
	_guiCustomOffsetUniform = _guiProgram->getUniformLocation("vCustomOffset");
	_guiCustomAlphaUniform = _guiProgram->getUniformLocation("customAlpha");
	_guiProgram->setUniform("layer0_texture", 0);
	_guiProgram->setUniform("layer1_texture", 1);

	setProjectionMatrices(rc->size());
}

void GuiRenderer::setProjectionMatrices(const vec2& contextSize)
{
	_defaultTransform = IDENTITY_MATRIX;
	_defaultTransform[0][0] = 2.0f / contextSize.x;
	_defaultTransform[1][1] = -2.0f / contextSize.y;
	_defaultTransform[3][0] = -1.0f;
	_defaultTransform[3][1] = 1.0f;
	_defaultTransform[3][3] = 1.0f;

	_guiCamera.perspectiveProjection(DEG_30, contextSize.aspect(), 1.0f, 100.0f);
	_guiCamera.lookAt(vec3(0.0f, 0.0f, cos(DEG_15) / sin(DEG_15)), vec3(0.0f), unitY);
}

void GuiRenderer::alloc(size_t count)
{
	if (_renderingElement.valid() && (_renderingElement->_vertexList.currentIndex() + count >= _renderingElement->_vertexList.size()))
	{
		size_t newSize = _renderingElement->_vertexList.size() + BlockSize;
		_renderingElement->_vertexList.resize(newSize);

		size_t currentActualSize = _renderingElement->_indexArray->actualSize();
		_renderingElement->_indexArray->resizeToFit(newSize);
		_renderingElement->_indexArray->linearize();
		_renderingElement->_indexArray->setActualSize(currentActualSize);
	}
}

GuiVertexPointer GuiRenderer::allocateVertices(size_t count, const Texture& texture, ElementClass cls, GuiRenderLayer layer)
{
	if (!_renderingElement.valid()) return 0;

	_renderingElement->_changed = true;
	size_t i0 = _renderingElement->_vertexList.currentIndex();

	bool shouldAdd = false;

	if (_renderingElement->_chunks.size())
	{
		RenderChunk& lastChunk = _renderingElement->_chunks.back();

		if ((lastChunk.elementClass == cls) && (lastChunk.layers[layer] == texture))
			lastChunk.count += count;
		else
			shouldAdd = true;
	}
	else
	{
		shouldAdd = true;
	}

	if (shouldAdd)
	{
		_lastTextures[layer] = texture;
		_renderingElement->_chunks.push_back(RenderChunk(i0, count, 
			_lastTextures[GuiRenderLayer_Layer0], _lastTextures[GuiRenderLayer_Layer1], cls));
	}
	alloc(count);
	_renderingElement->_vertexList.offset(count);

	return _renderingElement->_vertexList.element_ptr(i0);
}

size_t GuiRenderer::addVertices(const GuiVertexList& vertices, const Texture& texture, ElementClass cls, GuiRenderLayer layer)
{
	if (!_renderingElement.valid()) return 0;

	size_t current = _renderingElement->_vertexList.currentIndex();
	size_t count = vertices.currentIndex();

	if (count)
	{
		GuiVertex* v0 = allocateVertices(count, texture, cls, layer);
		memcpy(v0, vertices.raw(), vertices.dataSize());
	}
	else 
	{
		std::cout << "Adding empty vertices!" << std::endl;
	}

	return current;
}

void GuiRenderer::setRendernigElement(const RenderingElementRef& r)
{
	_renderingElement = r;
	_lastTextures[GuiRenderLayer_Layer0] = Texture();
	_lastTextures[GuiRenderLayer_Layer1] = Texture();
}

void GuiRenderer::beginRender(RenderContext* rc)
{
	rc->renderer()->clear(false, true);

	_depthTestEnabled = rc->renderState().depthTestEnabled();
	_blendEnabled = rc->renderState().blendEnabled();
	_blendState = rc->renderState().blendState();
	_depthMaskEnabled = rc->renderState().depthMaskEnabled();

	rc->renderState().setBlend(true, Blend_Default);
	rc->renderState().bindProgram(_guiProgram);
}

void GuiRenderer::endRender(RenderContext* rc)
{
	rc->renderState().setDepthTest(_depthTestEnabled);
	rc->renderState().setBlend(_blendEnabled, _blendState);
	rc->renderState().setDepthMask(_depthMaskEnabled);
}

void GuiRenderer::render(RenderContext* rc)
{
	if (!_renderingElement.valid()) return;

	RenderState& rs = rc->renderState();
	Renderer* renderer = rc->renderer();

	const VertexArrayObject& vao = _renderingElement->vertexArrayObject();

	_guiProgram->setUniform(_guiCustomOffsetUniform, GL_FLOAT_VEC2, _customOffset);
	_guiProgram->setUniform(_guiCustomAlphaUniform, GL_FLOAT, _customAlpha);
	ElementClass elementClass = ElementClass_max;
	for (RenderChunkList::const_iterator i = _renderingElement->_chunks.begin(), e = _renderingElement->_chunks.end(); i != e; ++i)
	{
		rs.bindTexture(0, i->layers[GuiRenderLayer_Layer0]);
		rs.bindTexture(1, i->layers[GuiRenderLayer_Layer1]);

		if (i->elementClass != elementClass)
		{
			elementClass = i->elementClass;
			bool is3D = i->elementClass == ElementClass_3d;
			rs.setDepthTest(is3D);
			rs.setDepthMask(is3D);
			_guiProgram->setUniform(_guiDefaultTransformUniform, GL_FLOAT_MAT4,
					is3D ? _guiCamera.modelViewProjectionMatrix() : _defaultTransform);
		}

		renderer->drawElements(vao->indexBuffer(), i->first, i->count);
	}
}

void GuiRenderer::buildQuad(GuiVertexList& vertices, const GuiVertex& topLeft, const GuiVertex& topRight, 
	const GuiVertex& bottomLeft, const GuiVertex& bottomRight)
{
	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);
	vertices.push_back(topRight);
	vertices.push_back(bottomLeft);
	vertices.push_back(topRight);
	vertices.push_back(topLeft);
}

void GuiRenderer::createStringVertices(GuiVertexList& vertices, const CharDescriptorList& chars, ElementAlignment hAlign, ElementAlignment vAlign, 
									   const vec2& pos, const vec4& color, const mat4& transform, GuiRenderLayer layer)
{
	vec4 line;
	std::vector<vec4> lines;
	
	for (CharDescriptorList::const_iterator i = chars.begin(), e = chars.end(); i != e; ++i)
	{
		const CharDescriptor& desc = *i;
		line.w = etMax(line.w, desc.size.y);
		
		if ((desc.value == ET_NEWLINE) || (desc.value == ET_RETURN))
		{
			lines.push_back(line);
			line = vec4(0.0f, line.y + line.w, 0.0f, 0.0f);
		}
		else 
		{
			line.z += desc.size.x;
		}
	}
	lines.push_back(line);
	
	if (hAlign == ElementAlignment_Center)
	{
		for (std::vector<vec4>::iterator i = lines.begin(), e = lines.end(); i != e; ++i)
			i->x = -0.5f * i->z;
	}
	else if (hAlign == ElementAlignment_Far)
	{
		for (std::vector<vec4>::iterator i = lines.begin(), e = lines.end(); i != e; ++i)
			i->x = -i->z;
	}
	
	if (vAlign != ElementAlignment_Near)
	{
	}
	
	size_t lineIndex = 0;
	line = lines.front();
	
	vec2 mask(layer == GuiRenderLayer_Layer0 ? 0.0f : 1.0f, 0.0f);
	vertices.fitToSize(6 * chars.size());
	for (CharDescriptorList::const_iterator i = chars.begin(), e = chars.end(); i != e; ++i)
	{
		const CharDescriptor& desc = *i;
		if ((desc.value == ET_NEWLINE) || (desc.value == ET_RETURN))
		{
			line = lines[++lineIndex];
		}
		else 
		{
			vec2 topLeft = line.xy() + pos;
			vec2 bottomLeft = topLeft + vec2(0.0f, desc.size.y);
			vec2 topRight = topLeft + vec2(desc.size.x, 0.0f);
			vec2 bottomRight = bottomLeft + vec2(desc.size.x, 0.0f);
			
			vec2 topLeftUV = desc.uvOrigin;
			vec2 topRightUV = topLeftUV + vec2(desc.uvSize.x, 0.0f);
			vec2 bottomLeftUV = desc.uvOrigin - vec2(0.0f, desc.uvSize.y);
			vec2 bottomRightUV = bottomLeftUV + vec2(desc.uvSize.x, 0.0f);
			vec4 charColor = desc.color * color;
			
			buildQuad(vertices,
					  GuiVertex(floorv(transform * topLeft), vec4(topLeftUV, mask), charColor),
					  GuiVertex(floorv(transform * topRight), vec4(topRightUV, mask), charColor),
					  GuiVertex(floorv(transform * bottomLeft), vec4(bottomLeftUV, mask), charColor),
					  GuiVertex(floorv(transform * bottomRight), vec4(bottomRightUV, mask), charColor));
			
			line.x += desc.size.x;
		}
	}
}

int GuiRenderer::measuseVerticesCountForImageDescriptor(const ImageDescriptor& desc)
{
	bool hasLeftSafe = desc.contentOffset.left > 0;
	bool hasTopSafe = desc.contentOffset.top > 0;
	bool hasRightSafe = desc.contentOffset.right > 0;
	bool hasBottomSafe = desc.contentOffset.bottom > 0;
	bool hasLeftTopCorner = hasLeftSafe && hasTopSafe;
	bool hasRightTopCorner = hasRightSafe && hasTopSafe;
	bool hasLeftBottomCorner = hasLeftSafe && hasBottomSafe;
	bool hasRightBottomCorner = hasRightSafe && hasBottomSafe;

	int numBorders = hasLeftSafe + hasTopSafe + hasRightSafe + hasBottomSafe;
	int numCorners = hasLeftTopCorner + hasRightTopCorner + hasLeftBottomCorner + hasRightBottomCorner;

	return 6 * (1 + numCorners + numBorders);
}

void GuiRenderer::createImageVertices(GuiVertexList& vertices, const Texture& tex, const ImageDescriptor& desc, 
	const rect& p, const vec4& color, const mat4& transform, GuiRenderLayer layer)
{
	if (!tex.valid()) return;

	bool hasLeftSafe = desc.contentOffset.left > 0;
	bool hasTopSafe = desc.contentOffset.top > 0;
	bool hasRightSafe = desc.contentOffset.right > 0;
	bool hasBottomSafe = desc.contentOffset.bottom > 0;
	bool hasLeftTopCorner = hasLeftSafe && hasTopSafe;
	bool hasRightTopCorner = hasRightSafe && hasTopSafe;
	bool hasLeftBottomCorner = hasLeftSafe && hasBottomSafe;
	bool hasRightBottomCorner = hasRightSafe && hasBottomSafe;

	int numBorders = hasLeftSafe + hasTopSafe + hasRightSafe + hasBottomSafe;
	int numCorners = hasLeftTopCorner + hasRightTopCorner + hasLeftBottomCorner + hasRightBottomCorner;

	vertices.fitToSize(6 * (1 + numCorners + numBorders));

	vec2 mask(layer == GuiRenderLayer_Layer0 ? 0.0f : 1.0f, 0.0f);

	vec2 topLeft = (p.origin());
	vec2 topRight = (topLeft + vec2(p.width, 0.0f));
	vec2 bottomLeft = (topLeft + vec2(0.0f, p.height));
	vec2 bottomRight = (bottomLeft + vec2(p.width, 0.0f));
	vec2 centerTopLeft = (p.origin() + desc.contentOffset.origin());
	vec2 centerTopRight = (p.origin() + vec2(p.width - desc.contentOffset.right, desc.contentOffset.top));
	vec2 centerBottomLeft = (p.origin() + vec2(desc.contentOffset.left, p.height - desc.contentOffset.bottom));
	vec2 centerBottomRight = (p.origin() + vec2(p.width - desc.contentOffset.right, p.height - desc.contentOffset.bottom));
	vec2 topCenterTopLeft = (topLeft + vec2(desc.contentOffset.left, 0.0f));
	vec2 topCenterTopRight = (topLeft + vec2(p.width - desc.contentOffset.right, 0));
	vec2 leftCenterTopLeft = (topLeft + vec2(0, desc.contentOffset.top));
	vec2 rightCenterTopRight = (topLeft + vec2(p.width, desc.contentOffset.top));
	vec2 leftCenterBottomLeft = (topLeft + vec2(0, p.height - desc.contentOffset.bottom));
	vec2 bottomCenterBottomLeft = (topLeft + vec2(desc.contentOffset.left, p.height));
	vec2 bottomCenterBottomRigth = (topLeft + vec2(p.width - desc.contentOffset.right, p.height));
	vec2 rightCenterBottomRigth = (topLeft + vec2(p.width, p.height - desc.contentOffset.bottom));

	vec2 topLeftUV = tex->getTexCoord( desc.origin );
	vec2 topRightUV = tex->getTexCoord( desc.origin + vec2(desc.size.x, 0.0f) );
	vec2 bottomLeftUV = tex->getTexCoord( desc.origin + vec2(0.0f, desc.size.y) );
	vec2 bottomRightUV = tex->getTexCoord( desc.origin + desc.size );
	vec2 centerTopLeftUV = tex->getTexCoord( desc.centerPartTopLeft() );
	vec2 centerBottomLeftUV = tex->getTexCoord( desc.centerPartBottomLeft() );
	vec2 centerTopRightUV = tex->getTexCoord( desc.centerPartTopRight() );
	vec2 centerBottomRightUV = tex->getTexCoord( desc.centerPartBottomRight() );
	vec2 topCenterTopLeftUV = tex->getTexCoord( desc.origin + vec2(desc.contentOffset.left, 0) );
	vec2 topCenterTopRightUV = tex->getTexCoord( desc.origin + vec2(desc.size.x - desc.contentOffset.right, 0) );
	vec2 leftCenterTopLeftUV = tex->getTexCoord( desc.origin + vec2(0, desc.contentOffset.top) );
	vec2 rightCenterTopRightUV = tex->getTexCoord( desc.origin + vec2(desc.size.x, desc.contentOffset.top) );
	vec2 leftCenterBottomLeftUV = tex->getTexCoord( desc.origin + vec2(0, desc.size.y - desc.contentOffset.bottom) );
	vec2 bottomCenterBottomLeftUV = tex->getTexCoord( desc.origin + vec2(desc.contentOffset.left, desc.size.y) );
	vec2 bottomCenterBottomRigthUV = tex->getTexCoord( desc.origin + vec2(desc.size.x - desc.contentOffset.right, desc.size.y) );
	vec2 rightCenterBottomRigthUV = tex->getTexCoord( desc.origin + vec2( desc.size.x, desc.size.y - desc.contentOffset.bottom));

	buildQuad(vertices, 
		GuiVertex(transform * centerTopLeft, vec4(centerTopLeftUV, mask), color ), 
		GuiVertex(transform * centerTopRight, vec4(centerTopRightUV, mask), color ),
		GuiVertex(transform * centerBottomLeft, vec4(centerBottomLeftUV, mask), color ),
		GuiVertex(transform * centerBottomRight, vec4(centerBottomRightUV, mask), color ) );

	if (hasLeftTopCorner)
	{
		buildQuad(vertices, 
			GuiVertex(transform * topLeft, vec4(topLeftUV, mask), color), 
			GuiVertex(transform * topCenterTopLeft, vec4(topCenterTopLeftUV, mask), color), 
			GuiVertex(transform * leftCenterTopLeft, vec4(leftCenterTopLeftUV, mask), color), 
			GuiVertex(transform * centerTopLeft, vec4(centerTopLeftUV, mask), color) );
	}

	if (hasRightTopCorner)
	{
		buildQuad(vertices,
			GuiVertex(transform * topCenterTopRight, vec4(topCenterTopRightUV, mask), color),
			GuiVertex(transform * topRight, vec4(topRightUV, mask), color), 
			GuiVertex(transform * centerTopRight, vec4(centerTopRightUV, mask), color), 
			GuiVertex(transform * rightCenterTopRight, vec4(rightCenterTopRightUV, mask), color) );
	}

	if (hasLeftBottomCorner)
	{
		buildQuad(vertices, 
			GuiVertex(transform * leftCenterBottomLeft, vec4(leftCenterBottomLeftUV, mask), color), 
			GuiVertex(transform * centerBottomLeft, vec4(centerBottomLeftUV, mask), color), 
			GuiVertex(transform * bottomLeft, vec4(bottomLeftUV, mask), color), 
			GuiVertex(transform * bottomCenterBottomLeft, vec4(bottomCenterBottomLeftUV, mask), color) );
	}

	if (hasRightBottomCorner)
	{
		buildQuad(vertices, 
			GuiVertex(transform * centerBottomRight, vec4(centerBottomRightUV, mask), color), 
			GuiVertex(transform * rightCenterBottomRigth, vec4(rightCenterBottomRigthUV, mask), color), 
			GuiVertex(transform * bottomCenterBottomRigth, vec4(bottomCenterBottomRigthUV, mask), color), 
			GuiVertex(transform * bottomRight, vec4(bottomRightUV, mask), color) );
	}

	if (hasTopSafe)
	{
		vec2 tl = hasLeftTopCorner ? topCenterTopLeft : topLeft;
		vec2 tr = hasRightTopCorner ? topCenterTopRight : topRight;
		vec2 bl = hasLeftTopCorner ? centerTopLeft : leftCenterTopLeft;
		vec2 br = hasRightTopCorner ? centerTopRight : rightCenterTopRight;
		vec2 tlUV = hasLeftTopCorner ? topCenterTopLeftUV : topLeftUV;
		vec2 trUV = hasRightTopCorner ? topCenterTopRightUV : topRightUV;
		vec2 blUV = hasLeftTopCorner ? centerTopLeftUV : leftCenterTopLeftUV;
		vec2 brUV = hasRightTopCorner ? centerTopRightUV : rightCenterTopRightUV;

		buildQuad(vertices, 
			GuiVertex(transform * tl, vec4(tlUV, mask), color),
			GuiVertex(transform * tr, vec4(trUV, mask), color),
			GuiVertex(transform * bl, vec4(blUV, mask), color), 
			GuiVertex(transform * br, vec4(brUV, mask), color) );
	}

	if (hasLeftSafe)
	{
		vec2 tl = hasLeftTopCorner ? leftCenterTopLeft : topLeft;
		vec2 tr = hasLeftTopCorner ? centerTopLeft : topCenterTopLeft;
		vec2 bl = hasLeftBottomCorner ? leftCenterBottomLeft : bottomLeft;
		vec2 br = hasLeftBottomCorner ? centerBottomLeft : bottomCenterBottomLeft;
		vec2 tlUV = hasLeftTopCorner ? leftCenterTopLeftUV : topLeftUV;
		vec2 trUV = hasLeftTopCorner ? centerTopLeftUV : topCenterTopLeftUV;
		vec2 blUV = hasLeftBottomCorner ? leftCenterBottomLeftUV : bottomLeftUV;
		vec2 brUV = hasLeftBottomCorner ? centerBottomLeftUV : bottomCenterBottomLeftUV;

		buildQuad(vertices,
			GuiVertex(transform * tl, vec4(tlUV, mask), color), 
			GuiVertex(transform * tr, vec4(trUV, mask), color),
			GuiVertex(transform * bl, vec4(blUV, mask), color),
			GuiVertex(transform * br, vec4(brUV, mask), color) );
	}

	if (hasBottomSafe)
	{
		vec2 tl = hasLeftBottomCorner ? centerBottomLeft : leftCenterBottomLeft;
		vec2 tr = hasRightBottomCorner ? centerBottomRight : rightCenterBottomRigth;
		vec2 bl = hasLeftBottomCorner ? bottomCenterBottomLeft : bottomLeft;
		vec2 br = hasRightBottomCorner ? bottomCenterBottomRigth : bottomRight;
		vec2 tlUV = hasLeftBottomCorner ? centerBottomLeftUV : leftCenterBottomLeftUV;
		vec2 trUV = hasRightBottomCorner ? centerBottomRightUV : rightCenterBottomRigthUV;
		vec2 blUV = hasLeftBottomCorner ? bottomCenterBottomLeftUV : bottomLeftUV;
		vec2 brUV = hasRightBottomCorner ? bottomCenterBottomRigthUV : bottomRightUV;

		buildQuad(vertices,
			GuiVertex(transform * tl, vec4(tlUV, mask), color), 
			GuiVertex(transform * tr, vec4(trUV, mask), color), 
			GuiVertex(transform * bl, vec4(blUV, mask), color), 
			GuiVertex(transform * br, vec4(brUV, mask), color) );
	}

	if (hasRightSafe)
	{
		vec2 tl = hasRightTopCorner ? centerTopRight : topCenterTopRight;
		vec2 tr = hasRightTopCorner ? rightCenterTopRight : topRight;
		vec2 bl = hasRightBottomCorner ? centerBottomRight : bottomCenterBottomRigth;
		vec2 br = hasRightBottomCorner ? rightCenterBottomRigth : bottomRight;
		vec2 tlUV = hasRightTopCorner ? centerTopRightUV : topCenterTopRightUV;
		vec2 trUV = hasRightTopCorner ? rightCenterTopRightUV : topRightUV;
		vec2 blUV = hasRightBottomCorner ? centerBottomRightUV : bottomCenterBottomRigthUV;
		vec2 brUV = hasRightBottomCorner ? rightCenterBottomRigthUV : bottomRightUV;

		buildQuad(vertices, 
			GuiVertex(transform * tl, vec4(tlUV, mask), color),
			GuiVertex(transform * tr, vec4(trUV, mask), color),
			GuiVertex(transform * bl, vec4(blUV, mask), color), 
			GuiVertex(transform * br, vec4(brUV, mask), color) );
	}
}

void GuiRenderer::createColorVertices(GuiVertexList& vertices, const rect& p, const vec4& color, 
									  const mat4& transform, GuiRenderLayer layer)
{
	vec2 topLeft = p.origin();
	vec2 topRight = topLeft + vec2(p.width, 0.0f);
	vec2 bottomLeft = topLeft + vec2(0.0f, p.height);
	vec2 bottomRight = bottomLeft + vec2(p.width, 0.0f);
	
	vec2 mask(layer == GuiRenderLayer_Layer0 ? 0.0f : 1.0f, 1.0f);
	
	buildQuad(vertices, 
			  GuiVertex(transform * topLeft, vec4(vec2(0.0f), mask), color ), 
			  GuiVertex(transform * topRight, vec4(vec2(0.0f), mask), color ),
			  GuiVertex(transform * bottomLeft, vec4(vec2(0.0f), mask), color ),
			  GuiVertex(transform * bottomRight, vec4(vec2(0.0f), mask), color ) );	
}

std::string gui_vertex_src = 
	"uniform mat4 mDefaultTransform;"
	"uniform vec2 vCustomOffset;"
	"uniform float customAlpha;"
	"etVertexIn vec3 Vertex;"
	"etVertexIn vec4 TexCoord0;"
	"etVertexIn vec4 Color;"
	"etVertexOut vec2 vTexCoord;"
	"etVertexOut float textureMask;"
	"etVertexOut vec4 additiveColor;"
	"etVertexOut vec4 vColor;"
	"void main()"
	"{"
	" vColor = Color;"
	" vColor.w *= customAlpha;"
	" vTexCoord = TexCoord0.xy;"
	" textureMask = TexCoord0.z;"
	" additiveColor = vColor * TexCoord0.w;"
	" vec4 vTransformed = mDefaultTransform * vec4(Vertex, 1.0);"
	" gl_Position = vTransformed + vec4(vTransformed.w * vCustomOffset, 0.0, 0.0);"
	"}";

std::string gui_frag_src = 
	"uniform etLowp sampler2D layer0_texture;"
	"uniform etLowp sampler2D layer1_texture;"
	"etFragmentIn etMediump vec2 vTexCoord;"
	"etFragmentIn etLowp float textureMask;"
	"etFragmentIn etLowp vec4 additiveColor;"
	"etFragmentIn etLowp vec4 vColor;"
	"void main()"
	"{"
	" etLowp vec4 textureColor = mix(etTexture2D(layer0_texture, vTexCoord), etTexture2D(layer1_texture, vTexCoord), textureMask);"
	" etFragmentOut = vColor * textureColor + additiveColor;"
	"}";
