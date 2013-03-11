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
	class Texture : public IntrusivePtr<TextureData>
	{
	public:
		Texture() : IntrusivePtr<TextureData>(0)
			{ }

		Texture(TextureData* data) : IntrusivePtr<TextureData>(data)
			{ }
	};

	typedef std::vector<Texture> TextureList;
	typedef std::map<std::string, Texture> TextureMap;
}

