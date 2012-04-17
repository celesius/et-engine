#include <et/core/tools.h>
#include <et/opengl/openglcaps.h>
#include <et/app/application.h>
#include <et/apiobjects/texture.h>
#include <et/device/rendercontext.h>

using namespace et;

static const int defaultBindingUnit = 7;

TextureData::TextureData(RenderContext* rc, const TextureDescription& desc, const std::string& id, bool deferred) : 
	APIObjectData(id), _glID(0), _desc(desc), _own(true)
{
	if (!deferred)
	{
		generateTexture(rc);
		build(rc);
	}
}

TextureData::TextureData(RenderContext*, GLuint texture, const vec2i& size, const std::string& name) : 
	APIObjectData(name), _glID(texture), _own(false)
{
	if (!glIsTexture(texture))
	{
		_glID = 0;
		return;
	}
	
	_desc.target = GL_TEXTURE_2D;
	_desc.size = size;
	_desc.mipMapCount = 1;
	_texel.x = 1.0f / static_cast<float>(size.x);
	_texel.y = 1.0f / static_cast<float>(size.y);
}

TextureData::~TextureData()
{
	if (_own && (_glID != 0) && glIsTexture(_glID))
		glDeleteTextures(1, &_glID);
}

void TextureData::setWrap(RenderContext* rc, TextureWrap s, TextureWrap t, TextureWrap r)
{
	_wrap = vector3<TextureWrap>(s, t, r);

	rc->renderState().bindTexture(defaultBindingUnit, _glID, _desc.target);

	glTexParameteri(_desc.target, GL_TEXTURE_WRAP_S, textureWrapValue(_wrap.x)); 
	checkOpenGLError("glTexParameteri<WRAP_S> " + name()); 
	glTexParameteri(_desc.target, GL_TEXTURE_WRAP_T, textureWrapValue(_wrap.y));
	checkOpenGLError("glTexParameteri<WRAP_T> " + name()); 
#if defined(GL_TEXTURE_WRAP_R)
	glTexParameteri(_desc.target, GL_TEXTURE_WRAP_R, textureWrapValue(_wrap.z));
	checkOpenGLError("glTexParameteri<WRAP_R> " + name()); 
#endif	
}

void TextureData::setFiltration(RenderContext* rc, TextureFiltration minFiltration, TextureFiltration magFiltration)
{
	rc->renderState().bindTexture(defaultBindingUnit, _glID, _desc.target);

	_filtration = vector2<TextureFiltration>(minFiltration, magFiltration);

	if ((_desc.mipMapCount < 2) && (minFiltration > TextureFiltration_Linear))
		_filtration.x = TextureFiltration_Linear;

	if (magFiltration > TextureFiltration_Linear)
		_filtration.y = TextureFiltration_Linear;

	glTexParameteri(_desc.target, GL_TEXTURE_MIN_FILTER, textureFiltrationValue(_filtration.x)); 
	checkOpenGLError("glTexParameteri<GL_TEXTURE_MIN_FILTER> " + name()); 
	glTexParameteri(_desc.target, GL_TEXTURE_MAG_FILTER, textureFiltrationValue(_filtration.y)); 
	checkOpenGLError("glTexParameteri<GL_TEXTURE_MAG_FILTER> " + name()); 
}

void TextureData::compareRefToTexture(RenderContext* rc, bool enable, GLenum compareFunc)
{
#if (!ET_OPENGLES)
	rc->renderState().bindTexture(defaultBindingUnit, _glID, _desc.target);

	if (enable)
	{
		glTexParameteri(_desc.target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		checkOpenGLError("glTexParameteri(_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE) " + name()); 

		glTexParameteri(_desc.target, GL_TEXTURE_COMPARE_FUNC, compareFunc);
		checkOpenGLError("glTexParameteri(_target, GL_TEXTURE_COMPARE_FUNC, compareFunc) " + name()); 
	}
	else
	{
		glTexParameteri(_desc.target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		checkOpenGLError("glTexParameteri(_target, GL_TEXTURE_COMPARE_MODE, GL_NONE) " + name()); 
	}
#else
	std::cout << "TextureData::compareRefToTexture" << rc << ", " << enable << ", " << compareFunc << ") call in OpenGL ES" << std::endl;
#endif	
}

void TextureData::generateTexture(RenderContext*)
{
	glGenTextures(1, &_glID);
	checkOpenGLError("TextureData::generateTexture " + name());
}

void TextureData::buildData(char* dataPtr, size_t dataSize)
{
	if (_desc.target == GL_TEXTURE_1D)
	{
		if (_desc.compressed && dataSize)
			etCompressedTexImage1D(_desc.target, 0, _desc.internalformat, _desc.size.x, 0, dataSize, dataPtr); 
		else
			etTexImage1D(_desc.target, 0, _desc.internalformat, _desc.size.x, 0, _desc.format, _desc.type, dataPtr); 
        
	}
	else if (_desc.target == GL_TEXTURE_2D)
	{
		for (size_t level = 0; level < _desc.mipMapCount; ++level)
		{
			vec2i mipSize = _desc.sizeForMipLevel(level);
			size_t dataSize = _desc.dataSizeForMipLevel(level);
			size_t offset = _desc.dataOffsetForMipLevel(level);
			char* ptr = (dataSize > 0) ? &dataPtr[offset] : 0;
			if (_desc.compressed && ptr)
				etCompressedTexImage2D(_desc.target, level, _desc.internalformat, mipSize.x, mipSize.y, 0, dataSize, ptr); 
			else
				etTexImage2D(_desc.target, level, _desc.internalformat, mipSize.x, mipSize.y, 0, _desc.format, _desc.type, ptr);
		}
	}
	else if (_desc.target == GL_TEXTURE_CUBE_MAP)
	{
		size_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		for (size_t layer = 0; layer < _desc.layersCount; ++layer, ++target)
		{
			for (size_t level = 0; level < _desc.mipMapCount; ++level)
			{
				vec2i mipSize = _desc.sizeForMipLevel(level);
				size_t dataSize = _desc.dataSizeForMipLevel(level);
				size_t offset = _desc.dataOffsetForMipLevel(level, layer);
				char* ptr = (offset < dataSize) ? &dataPtr[offset] : 0;
				if (_desc.compressed && ptr)
					etCompressedTexImage2D(target, level, _desc.internalformat, mipSize.x, mipSize.y, 0, dataSize, ptr); 
				else
					etTexImage2D(target, level, _desc.internalformat, mipSize.x, mipSize.y, 0, _desc.format, _desc.type, ptr);
			}
		}
	}
	else
	{
		std::cout << "Unsupported texture target specified: glTexTargetToString(_target)" << std::endl;
	}
}

void TextureData::build(RenderContext* rc)
{
	checkOpenGLError("TextureData::buildTexture2D " + name());
	if ((_desc.size.square() == 0) || (_desc.internalformat == 0) || (_desc.type == 0)) return;

	_texel = vec2( 1.0f / static_cast<float>(_desc.size.x), 1.0f / static_cast<float>(_desc.size.y) );
	_filtration.x = (_desc.mipMapCount > 1) ? TextureFiltration_LinearMipMapLinear : TextureFiltration_Linear;
	_filtration.y = TextureFiltration_Linear;

	rc->renderState().bindTexture(defaultBindingUnit, _glID, _desc.target);
	checkOpenGLError("TextureData::buildTexture2D -> etBindTexture" + name());

	glTexParameteri(_desc.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkOpenGLError("TextureData::buildTexture2D -> glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) for " + name());

	setFiltration(rc, _filtration.x, _filtration.y);
	setWrap(rc, _wrap.x, _wrap.y, _wrap.z);

	if (_desc.mipMapCount > 1)
	{
		glTexParameteri(_desc.target, GL_TEXTURE_MAX_LEVEL, _desc.mipMapCount - 1);
		checkOpenGLError("TextureData::buildTexture2D -> glTexParameteri(_desc.target, GL_TEXTURE_MAX_LEVEL, _desc.mipMapCount - 1) for " + name());
	} 

    buildData(_desc.data.binary(), _desc.data.dataSize());
	_desc.data.resize(0);
}

vec2 TextureData::getTexCoord(const vec2& vec, TextureOrigin origin) const
{
	float ax = vec.x * _texel.x;
	float ay = vec.y * _texel.y;
	return vec2(ax, (origin == TextureOrigin_TopLeft) ? 1.0f - ay : ay);
}

void TextureData::updateData(RenderContext* rc, const TextureDescription& desc)
{
	if (_glID == 0)
		generateTexture(rc);

	_desc = desc;
	build(rc);
}

void TextureData::updateDataDirectly(RenderContext* rc, const vec2i& size, char* data, size_t dataSize)
{
	if (_glID == 0)
		generateTexture(rc);

    _desc.size = size;
    rc->renderState().bindTexture(defaultBindingUnit, _glID, _desc.target);
	buildData(data, dataSize);
}