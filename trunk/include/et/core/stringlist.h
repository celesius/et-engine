/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#if (ET_SUPPORT_INITIALIZER_LIST)
#
	#define ET_DECLARE_STRINGLIST(NAME, ...) StringList NAME = { __VA_ARGS__ };
#
#else
#
	#define ET_DECLARE_STRINGLIST(NAME, ...) \
		std::string et##NAME##strings[] = {__VA_ARGS__};\
		size_t etnum##NAME##strings = sizeof(et##NAME##strings) / sizeof(std::string); \
		StringList NAME = StringList(et##NAME##strings, et##NAME##strings + etnum##NAME##strings); 
#
#endif

namespace et
{
	typedef std::vector<std::string> StringList;
}
