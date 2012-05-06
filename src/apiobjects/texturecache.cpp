/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/apiobjects/texturecache.h>

using namespace et;

TextureCache::~TextureCache()
{
	clear();
}

void TextureCache::manageTexture(const Texture& tex)
{
	CriticalSectionScope lock(_lock);

	if (tex.valid())
		_textures[tex->name()] = tex;
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

	TextureMap::iterator i = _textures.begin();
	while (i != _textures.end())
	{
		if (i->second->referenceCount() == 1)
		{
			TextureMap::iterator toErase = i;
			++i;
			_textures.erase(toErase);
		}
		else
		{
			++i;
		}
	}
}

void TextureCache::clear()
{
	CriticalSectionScope lock(_lock);
	_textures.clear();
}