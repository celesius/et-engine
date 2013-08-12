/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/objectscache.h>
#include <et/rendering/rendercontext.h>
#include <et/opengl/openglcaps.h>
#include <et/threading/threading.h>
#include <et/resources/textureloader.h>
#include <et/apiobjects/texturefactory.h>
#include <et/app/application.h>

using namespace et;

TextureFactory::TextureFactory(RenderContext* rc) :
	APIObjectFactory(rc)
{
	_loader = ObjectLoader::Pointer(this);
	_loadingThread = new TextureLoadingThread(this);
	retain();
}

TextureFactory::~TextureFactory()
{
	_loadingThread->stop();
	_loadingThread->waitForTermination();
	
	_loader.reset(nullptr);
	release();
}

Texture TextureFactory::loadTexture(const std::string& fileName, ObjectsCache& cache,
	bool async, TextureLoaderDelegate* delegate)
{
	if (fileName.length() == 0)
		return Texture();
	
	CriticalSectionScope lock(_csTextureLoading);
	
	std::string file = application().environment().resolveScalableFileName(fileName,
		renderContext()->screenScaleFactor());
    
    Texture texture = cache.find(file);
	if (texture.invalid())
	{
		TextureDescription::Pointer desc =
			async ? et::loadTextureDescription(file, false) : et::loadTexture(file);

		if (desc.valid())
		{
			bool calledFromAnotherThread = Threading::currentThread() != threading().renderingThread();
			
			texture = Texture(new TextureData(renderContext(), desc, desc->origin(), calledFromAnotherThread));
			cache.manage(texture, ObjectLoader::Pointer(this));
			
			if (async)
				_loadingThread->addRequest(desc->origin(), texture, delegate);
			else if (calledFromAnotherThread)
				assert(false && "ERROR: Unable to load texture synchronously from non-rendering thread.");
		}
		
	}
	else if (async && (delegate != nullptr))
	{
		Invocation1 i;

		i.setTarget(delegate, &TextureLoaderDelegate::textureDidStartLoading, texture);
		i.invokeInMainRunLoop();

		i.setTarget(delegate, &TextureLoaderDelegate::textureDidLoad, texture);
		i.invokeInMainRunLoop();
	}
   
	return texture;
}

Texture TextureFactory::genTexture(uint32_t target, uint32_t internalformat, const vec2i& size,
	uint32_t format, uint32_t type, const BinaryDataStorage& data, const std::string& id)
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
	
	if (format == GL_RGBA)
	{
		desc->bitsPerPixel = 32;
	}
	else if (format == GL_RGB)
	{
		desc->bitsPerPixel = 24;
	}
	else if (format == GL_DEPTH_COMPONENT)
	{
		desc->bitsPerPixel = 16;
	}
#if defined(GL_BGRA)
	else if (format == GL_BGRA)
	{
		desc->bitsPerPixel = 32;
	}
#endif
	else
	{
		assert(false && "Unsupported format for TextureFactory::genTexture method.");
	}

	return Texture(new TextureData(renderContext(), desc, id, false));
}

Texture TextureFactory::genCubeTexture(uint32_t internalformat, GLsizei size, uint32_t format, uint32_t type,
	const std::string& id)
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
	return Texture(new TextureData(renderContext(), desc, desc->origin(), false));
}

Texture TextureFactory::genNoiseTexture(const vec2i& size, bool norm, const std::string& id)
{
	const float RAND_MAXF = static_cast<float>(RAND_MAX);

	DataStorage<vec4ub> randata(size.square());
	for (size_t i = 0; i < randata.size(); ++i)
	{ 
		vec4 rand_f = vec4(static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF,
						   static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF);
		rand_f = 2.0f * rand_f - vec4(1.0f);
		randata[i] = vec4f_to_4ub(norm ? vec4(rand_f.xyz().normalize(), rand_f.w) : rand_f);
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

Texture TextureFactory::loadTexturesToCubemap(const std::string& posx, const std::string& negx,
	const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz,
	ObjectsCache& cache)
{
	TextureDescription::Pointer layers[6] = 
	{
		et::loadTexture(posx),
		et::loadTexture(negx),
		et::loadTexture(negy),
		et::loadTexture(posy),
		et::loadTexture(posz),
		et::loadTexture(negz)
	};

	int maxCubemapSize = openGLCapabilites().maxCubemapTextureSize();
	
	for (size_t l = 0; l < 6; ++l)
	{
		if (layers[l].valid())
		{
			if ((layers[l]->size.x > maxCubemapSize) || (layers[l]->size.y > maxCubemapSize))
			{
				log::error("Cubemap %s size of (%d x %d) is larger than allowed %dx%d",
					layers[l]->origin().c_str(), layers[l]->size.x, layers[l]->size.y, maxCubemapSize, maxCubemapSize);
				return Texture();
			}
		}
		else
		{
			log::error("Unable to load cubemap face.");
			return Texture();
		}
	}

	std::string texId = layers[0]->origin() + ";";
	for (size_t l = 1; l < 6; ++l)
	{
		texId += (l < 5) ? layers[l]->origin() + ";" : layers[l]->origin();
		if ((layers[l-1]->size != layers[l]->size) || 
			(layers[l-1]->format != layers[l]->format) ||
			(layers[l-1]->internalformat != layers[l]->internalformat) || 
			(layers[l-1]->type != layers[l]->type) || 
			(layers[l-1]->mipMapCount != layers[l]->mipMapCount) || 
			(layers[l-1]->compressed != layers[l]->compressed) ||
			(layers[l-1]->data.size() != layers[l]->data.size()))
		{
			log::error("Failed to load cubemap textures. Textures `%s` and `%s` aren't identical",
				layers[l-1]->origin().c_str(), layers[l]->origin().c_str());
			return Texture();
		}
	}

	size_t layerSize = layers[0]->dataSizeForAllMipLevels();
	TextureDescription::Pointer desc(new TextureDescription);
	desc->target = GL_TEXTURE_CUBE_MAP;
	desc->layersCount = 6;
	desc->bitsPerPixel = layers[0]->bitsPerPixel;
	desc->channels = layers[0]->channels;
	desc->compressed = layers[0]->compressed;
	desc->format = layers[0]->format;
	desc->internalformat = layers[0]->internalformat;
	desc->mipMapCount= layers[0]->mipMapCount;
	desc->size = layers[0]->size;
	desc->type = layers[0]->type;
	desc->data.resize(desc->layersCount * layerSize);
	for (size_t l = 0; l < desc->layersCount; ++l)
		etCopyMemory(desc->data.element_ptr(l * layerSize), layers[l]->data.element_ptr(0), layerSize);

	Texture result(new TextureData(renderContext(), desc, texId, false));
	
	for (size_t i = 0; i < 6; ++i)
		result->addOrigin(layers[i]->origin());
	
	cache.manage(result, ObjectLoader::Pointer(this));
	
	return result;
}

Texture TextureFactory::createTextureWrapper(uint32_t texture, const vec2i& size, const std::string& name)
{
	return Texture(new TextureData(renderContext(), texture, size, name));
}

void TextureFactory::reloadObject(LoadableObject::Pointer object, ObjectsCache&)
{
	TextureDescription::Pointer newData = et::loadTexture(object->origin());
	if (newData.valid())
		Texture(object)->updateData(renderContext(), newData);
}
