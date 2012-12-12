/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/rendering/rendercontext.h>
#include <et/threading/threading.h>
#include <et/apiobjects/texturefactory.h>
#include <et/resources/textureloader.h>

using namespace et;

TextureFactory::TextureFactory(RenderContext* rc) : APIObjectFactory(rc)
{
	_loadingThread = new TextureLoadingThread(this);
}

TextureFactory::~TextureFactory()
{
	_loadingThread->terminate();
}

Texture TextureFactory::loadTexture(const std::string& file, TextureCache& cache, bool async, TextureLoaderDelegate* delegate)
{
	if (file.length() == 0) return Texture();

	CriticalSectionScope lock(_csTextureLoading);
    
    Texture texture = cache.findTexture(file);
	if (texture.invalid())
	{
		bool calledFromAnotherThread = Threading::currentThread() != threading().mainThread();
		size_t screenScale = renderContext()->screenScaleFactor();
		
		TextureDescription::Pointer desc = async ? 
            TextureLoader::loadDescription(file, screenScale, false) :
			TextureLoader::load(file, screenScale);

		if (desc.valid())
		{
			texture = Texture(new TextureData(renderContext(), desc, desc->source, calledFromAnotherThread));
			cache.manageTexture(texture);
			
			if (async)
			{
				_loadingThread->addRequest(desc->source, renderContext()->screenScaleFactor(), texture, delegate);

				Invocation1 i;
				i.setTarget(delegate, &TextureLoaderDelegate::textureDidStartLoading, texture);
				i.invokeInMainRunLoop();
			}
			else if (calledFromAnotherThread)
			{
				std::cout << "ERROR: Unable to load texture synchronously from secondary thread." << std::endl;
			}
		}
		
	}
	else if (async && delegate)
	{
		Invocation1 i;

		i.setTarget(delegate, &TextureLoaderDelegate::textureDidStartLoading, texture);
		i.invokeInMainRunLoop();

		i.setTarget(delegate, &TextureLoaderDelegate::textureDidLoad, texture);
		i.invokeInMainRunLoop();
	}
   
	return texture;
}

Texture TextureFactory::genTexture(GLenum target, GLint internalformat, const vec2i& size, GLenum format, 
	GLenum type, const BinaryDataStorage& data, const std::string& id)
{
	TextureDescription::Pointer desc(new TextureDescription);
	desc->target = target;
	desc->data = data;
	desc->format = format;
	desc->internalformat = internalformat;
	desc->type = type;
	desc->size = size;
	desc->mipMapCount = 1;
	desc->layersCount = 1;
    // TODO: fix to actual values
    desc->bitsPerPixel = format == GL_RGBA ? 32 : 24;
	return Texture(new TextureData(renderContext(), desc, id, false));
}

Texture TextureFactory::genCubeTexture(GLint internalformat, GLsizei size, GLenum format, GLenum type, const std::string& id)
{
	TextureDescription::Pointer desc(new TextureDescription);
	desc->target = GL_TEXTURE_CUBE_MAP;
	desc->format = format;
	desc->internalformat = internalformat;
	desc->type = type;
	desc->size = vec2i(size);
	desc->layersCount = 6;
	desc->mipMapCount = 1;

	return Texture(new TextureData(renderContext(), desc, id, false));
}

Texture TextureFactory::genTexture(TextureDescription::Pointer desc)
{
	return Texture(new TextureData(renderContext(), desc, desc->source, false));
}

Texture TextureFactory::genNoiseTexture(const vec2i& size, bool norm, const std::string& id)
{
	const float RAND_MAXF = static_cast<float>(RAND_MAX);

	DataStorage<vec4ub> randata(size.square());
	for (size_t i = 0; i < randata.size(); ++i)
	{ 
		vec4 rand_f = 2.0f * vec4(static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF, 
			static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF ) - vec4(1.0);

		if (norm)
			rand_f.xyz() = normalize( rand_f.xyz() );

		randata.push_back(vec4f_to_4ub(rand_f));
	}

	TextureDescription::Pointer desc(new TextureDescription);
	desc->data = BinaryDataStorage(size.square() * 4);
	desc->target = GL_TEXTURE_2D;
	desc->format = GL_RGBA;
	desc->internalformat = GL_RGBA;
	desc->type = GL_UNSIGNED_BYTE;
	desc->size = size;
	desc->mipMapCount = 1;
	desc->layersCount = 1;
    desc->bitsPerPixel = 32;
    
	etCopyMemory(desc->data.data(), randata.data(), randata.dataSize());

	return Texture(new TextureData(renderContext(), desc, id, false));
}

void TextureFactory::textureLoadingThreadDidLoadTextureData(TextureLoadingRequest* request)
{
	CriticalSectionScope lock(_csTextureLoading);

	request->texture->updateData(renderContext(), request->textureDescription);
	textureDidLoad.invoke(request->texture);

	if (request->delegate)
		request->delegate->textureDidLoad(request->texture);

	delete request;
}

Texture TextureFactory::loadTexturesToCubemap(const std::string& posx, const std::string& negx, const std::string& posy,
	const std::string& negy, const std::string& posz, const std::string& negz, TextureCache& cache)
{
	size_t screenScale = renderContext()->screenScaleFactor();
	TextureDescription::Pointer layers[6] = 
	{
		TextureLoader::load(posx, screenScale), 
		TextureLoader::load(negx, screenScale), 
		TextureLoader::load(negy, screenScale), 
		TextureLoader::load(posy, screenScale), 
		TextureLoader::load(posz, screenScale), 
		TextureLoader::load(negz, screenScale) 
	};

	std::string texId = layers[0]->source + ";";
	for (size_t l = 1; l < 6; ++l)
	{
		texId += (l < 5) ? layers[l]->source + ";" : layers[l]->source;
		if ((layers[l-1]->size != layers[l]->size) || 
			(layers[l-1]->format != layers[l]->format) ||
			(layers[l-1]->internalformat != layers[l]->internalformat) || 
			(layers[l-1]->type != layers[l]->type) || 
			(layers[l-1]->mipMapCount != layers[l]->mipMapCount) || 
			(layers[l-1]->compressed != layers[l]->compressed) ||
			(layers[l-1]->data.size() != layers[l]->data.size()))
		{
			std::cout << "Failed to load cubemap textures. Textures aren't identical:" << std::endl <<
				"> " << layers[l-1]->source << std::endl <<
				"> " << layers[l]->source << std::endl;
			return Texture();
		}
	}

	size_t layerSize = layers[0]->dataSizeForAllMipLevels();
	TextureDescription::Pointer desc(new TextureDescription);
	desc->target = GL_TEXTURE_CUBE_MAP;
	desc->layersCount = 6;
	desc->data.resize(6 * layerSize);

	for (size_t l = 0; l < 6; ++l)
	{
		etCopyMemory(desc->data.element_ptr(l * layerSize), layers[l]->data.element_ptr(0), layerSize);
	}
	
	desc->bitsPerPixel = layers[0]->bitsPerPixel;
	desc->channels = layers[0]->channels;
	desc->compressed = layers[0]->compressed;
	desc->format = layers[0]->format;
	desc->internalformat = layers[0]->internalformat;
	desc->mipMapCount= layers[0]->mipMapCount;
	desc->size = layers[0]->size;
	desc->type = layers[0]->type;

	return cache.manageTexture(Texture(new TextureData(renderContext(), desc, texId, false)));
}

Texture TextureFactory::createTextureWrapper(GLuint texture, const vec2i& size, const std::string& name)
{
	return Texture(new TextureData(renderContext(), texture, size, name));
}