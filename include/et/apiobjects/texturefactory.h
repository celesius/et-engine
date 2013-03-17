/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/app/events.h>
#include <et/threading/criticalsection.h>
#include <et/apiobjects/apiobjectfactory.h>
#include <et/apiobjects/texture.h>
#include <et/apiobjects/texturecache.h>
#include <et/apiobjects/textureloadingthread.h>

namespace et
{
	class TextureFactory : public APIObjectFactory, public TextureLoadingThreadDelegate
	{
	public:
		Texture loadTexture(const std::string& file, TextureCache& cache, bool async = false,
			TextureLoaderDelegate* delegate = 0);

		Texture loadTexturesToCubemap(const std::string& posx, const std::string& negx,
			const std::string& posy, const std::string& negy, const std::string& posz,
			const std::string& negz, TextureCache& cache);

		Texture genNoiseTexture(const vec2i& size, bool normalize, const std::string& id = "");
		Texture genCubeTexture(GLint internalformat, GLsizei size, GLenum format, GLenum type,
			const std::string& id = "");
		
		Texture genTexture(TextureDescription::Pointer desc);
		Texture genTexture(GLenum target, GLint internalformat, const vec2i& size, GLenum format,
			GLenum type, const BinaryDataStorage& data, const std::string& id = "");
		
		Texture createTextureWrapper(GLuint texture, const vec2i& size, const std::string& name = "");

		void textureLoadingThreadDidLoadTextureData(TextureLoadingRequest* request);
		
		ET_DECLARE_EVENT1(textureDidLoad, Texture)

	private:
		friend class RenderContext;
		
		TextureFactory(RenderContext*);

		TextureFactory(const TextureFactory&) : APIObjectFactory(0)
			{ }

		TextureFactory& operator = (const TextureFactory&)
			{ return *this; }

	private:
		AutoPtr<TextureLoadingThread> _loadingThread;
		CriticalSection _csTextureLoading;
	};

}