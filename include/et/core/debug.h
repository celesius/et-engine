/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <iostream>
#include <et/platform/platform.h>

namespace et
{
	template <typename T>
	inline void setDebugVariable_template_base(T& variable, const T& newValue, const char* variableName,
		const char* valueName, const char* function)
	{
		variable = newValue;
		std::cout << variableName << " = " << valueName << ", call from " << function << std::endl;
	}
}

#if (ET_DEBUG)
	#define setDebugVariable(var, val) et::setDebugVariable_template_base(var, val, #var, #val, __FUNCTION__);
#else
	#define setDebugVariable(var, val) var = val;
#endif

#define ET_TOCONSTCHAR_IMPL(a)	#a
#define ET_TOCONSTCHAR(a)		ET_TOCONSTCHAR_IMPL(a)

#if (ET_PLATFORM_WIN)

	#define ET_CALL_FUNCTION	__FUNCTION__

#elif (ET_PLATFORM_APPLE)

	#define ET_CALL_FUNCTION __PRETTY_FUNCTION__

#else

	#error ET_CALL_FUNCTION is undefined

#endif