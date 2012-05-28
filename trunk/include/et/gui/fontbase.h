/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <map>
#include <vector>
#include <et/core/containers.h>
#include <et/geometry/geometry.h>

#define FONT_VERSION_1			0x0001
#define FONT_VERSION_2			0x0002
#define FONT_VERSION_CURRENT	FONT_VERSION_2

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
			int value;
			int params;
			vec4 color;
			vec2 origin;
			vec2 size;
			vec2 uvOrigin;
			vec2 uvSize;
			vec4i extra;
			
			CharDescriptor() : value(0), params(0) 
				{ }
			
			CharDescriptor(unsigned short c, unsigned short p = 0) : value(c), params(p)
				{ }
			
			CharDescriptor(unsigned short c, unsigned short p, const vec2& sz) : value(c), params(p), size(sz)
				{ }
		};


		typedef std::vector<int> CharacterRange;
		typedef std::vector<CharDescriptor> CharDescriptorList;
		typedef std::map<int, CharDescriptor> CharDescriptorMap;
	}
}