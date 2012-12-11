/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/platform/platform.h>

#if (ET_DEBUG)
#	define setDebugVariable(var, val) et::et_internal_setDebugVariable(var, val, #var, #val, ET_CALL_FUNCTION);
#else
#	define setDebugVariable(var, val) var = val;
#endif

#if (ET_PLATFORM_WIN)
#
#	define ET_CALL_FUNCTION	__FUNCTION__
#
#	define _CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#
#	if ((_MSC_VER >= 1700) && !defined(ET_MSC_USE_1600_TOOLSET))
#		define ET_SUPPORT_RANGE_BASED_FOR	1
#	else
#		define ET_SUPPORT_RANGE_BASED_FOR	0
#	endif
#
#elif (ET_PLATFORM_APPLE)
#
#	define ET_CALL_FUNCTION				__PRETTY_FUNCTION__
#	define ET_SUPPORT_RANGE_BASED_FOR	__has_feature(cxx_range_for)
#
#else
#
#	error ET_CALL_FUNCTION is undefined
#
#endif

#if !defined(ET_SUPPORT_RANGE_BASED_FOR)
#
#	error Please define if compiler supports range based for
#
#endif

#if (ET_SUPPORT_RANGE_BASED_FOR)
#
#	define ET_START_ITERATION(container, type, variable)	for (type variable : container) {
#
#else 
#
#	define ET_START_ITERATION(container, type, variable)	for (auto variable##I = container.begin(), variable##E = container.end();\
#																variable##I != variable##E; ++variable##I) { type variable = *variable##I;
#endif

#define ET_END_ITERATION(container)							}

#define ET_ITERATE(container, type, variable, expression)	ET_START_ITERATION(container, type, variable)\
																expression;\
															ET_END_ITERATION(container)

namespace et
{
	template <typename T>
	inline void et_internal_setDebugVariable(T& variable, const T& newValue,
											 const char* variableName, const char* valueName, const char* function)
	{
		variable = newValue;
		std::cout << variableName << " = " << valueName << ", call from " << function << std::endl;
	}
}
