/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/apiobjects/texturecache.h>

using namespace et;

TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
	clear();
}

Texture TextureCache::manageTexture(const Texture& tex)
{
	CriticalSectionScope lock(_lock);

	if (tex.valid())
		_textures[tex->name()] = tex;

	return tex;
}

Texture TextureCache::findTexture(const std::string& key)
{
	CriticalSectionScope lock(_lock);

	TextureMap::iterator i = _textures.find(key);
	return (i == _textures.end()) ? Texture() : i->second;
}

void TextureCache::discardTexture(const Texture& obj)
{
	CriticalSectionScope lock(_lock);

	TextureMap::iterator i = _textures.begin();
	while (i != _textures.end())
	{
		if (i->first == obj->name())
		{
			TextureMap::iterator toErase = i;
			_textures.erase(toErase);
			++i;
			break;
		}
		else
		{
			++i;
		}
	}
}

void TextureCache::flush()
{
	CriticalSectionScope lock(_lock);

	size_t erased = 0;
	
	TextureMap::iterator i = _textures.begin();
	while (i != _textures.end())
	{
		if (i->second->atomicCounterValue() == 1)
		{
			TextureMap::iterator toErase = i;
			++i;
			_textures.erase(toErase);
			++erased;
		}
		else
		{
			++i;
		}
	}
	
	if (erased)
		log::info("[TextureCache] %lu textures flushed.", erased);
}

void TextureCache::clear()
{
	CriticalSectionScope lock(_lock);
	_textures.clear();
}