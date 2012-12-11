/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/apiobjects/texturedata.h>

namespace et
{

	class DDSLoader
	{
	public:
		static void loadFromStream(std::istream& stream, TextureDescription& desc);
		static void loadFromFile(const std::string& path, TextureDescription& desc);

		static void loadInfoFromStream(std::istream& stream, TextureDescription& desc);
		static void loadInfoFromFile(const std::string& path, TextureDescription& desc);
	};

#	include <et/imaging/ddsloader.const.h>	
}