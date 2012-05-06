/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/opengl/opengl.h>
#include <et/apiobjects/texture.h>

namespace et
{
	class TextureLoader
	{
	public:
		static const TextureDescription loadDescription(const std::string& name, size_t scaleFactor, bool initWithZero);
		static const TextureDescription load(const std::string& name, size_t scaleFactor);

		static const StringList& preferredExtensions();

		static std::string resolveScalableFileName(const std::string& name, size_t screenScale, std::string* ext = 0, bool silent = false);
		static std::string resolveFileName(const std::string& name, std::string* ext = 0, bool silent = false);
	};
}