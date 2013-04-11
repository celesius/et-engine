/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/threading/criticalsection.h>
#include <et/apiobjects/texture.h>

namespace et
{
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();

		Texture manageTexture(const Texture& tex);
		Texture findTexture(const std::string& key);
		
		void discardTexture(const Texture& tex);

		void clear();
		void flush();

	private:
		ET_DENY_COPY(TextureCache)

	private:
		typedef std::map<const std::string, Texture> TextureMap;

		CriticalSection _lock;
		TextureMap _textures;
	};
}