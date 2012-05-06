/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/containers.h>

#define FONT_VERSION_1			0x0001
#define FONT_VERSION_CURRENT	FONT_VERSION_1

namespace et
{
	namespace gui
	{
		enum CharParameter
		{
			CharParameter_Bold = 0x0001
		};

		struct CharDescriptor
		{
			unsigned short value;
			unsigned short params;
			vec4 color;
			vec2 origin;
			vec2 size;
			vec2 uvOrigin;
			vec2 uvSize;
			vec4i extra;
		};


		typedef std::vector<unsigned short> CharacterRange;
		typedef std::vector<CharDescriptor> CharDescriptorList;
		typedef std::map<unsigned short, CharDescriptor> CharDescriptorMap;
	}
}