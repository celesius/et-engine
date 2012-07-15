/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <string>
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

		void manageTexture(const Texture& tex);
		Texture findTexture(const std::string& key);
		void discardTexture(const Texture& tex);

		void clear();
		void flush();

	private:
		TextureCache(TextureCache&&)
			{ }

		TextureCache(const TextureCache&)
			{ }

		TextureCache& operator = (const TextureCache&)
			{ return *this; }

	private:
		typedef std::map<const std::string, Texture> TextureMap;

		CriticalSection _lock;
		TextureMap _textures;
	};
}