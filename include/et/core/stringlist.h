/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#if (ET_SUPPORT_INITIALIZER_LIST)
#
#	define ET_STRINGLIST(...)					StringList({ __VA_ARGS__ })
#
#else
#
#	define ET_STRINGLIST(...)					([&]()->StringList{const std::string et_sl[]={__VA_ARGS__};\
													return StringList(et_sl, et_sl+sizeof(et_sl)/sizeof(std::string));})()
#
#
#endif

namespace et
{
	typedef std::vector<std::string> StringList;
}
