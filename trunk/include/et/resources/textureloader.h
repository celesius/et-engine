/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
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
		static TextureDescription::Pointer loadDescription(const std::string& name,
			size_t scaleFactor, bool initWithZero);
		
		static TextureDescription::Pointer load(const std::string& name, size_t scaleFactor);

		static const StringList& preferredExtensions();

		static std::string resolveScalableFileName(const std::string& name, size_t screenScale,
			std::string* ext = nullptr, bool silent = false);
		
		static std::string resolveFileName(const std::string& name, std::string* ext = nullptr,
			bool silent = false);
	};
}