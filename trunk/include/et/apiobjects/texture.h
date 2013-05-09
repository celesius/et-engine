/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/texturedata.h>

namespace et
{
	typedef IntrusivePtr<TextureData> Texture;
	typedef std::vector<Texture> TextureList;
	typedef std::map<std::string, Texture> TextureMap;
}

