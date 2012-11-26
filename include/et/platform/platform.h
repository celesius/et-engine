/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <stddef.h>

#if defined(__MACH__)
	#include <TargetConditionals.h>
#endif

#if defined(_WIN32) || defined(_WIN64)

	#define ET_PLATFORM_WIN				1
	#define CurrentPlatform				Platform_Windows

	#if defined(_WIN64)
		#define ET_PLATFORM_WIN32		0
		#define ET_PLATFORM_WIN64		1
		#define ET_PLATFORM				ET_PLATFORM_WIN64
	#else
		#define ET_PLATFORM_WIN32		1
		#define ET_PLATFORM_WIN64		0
		#define ET_PLATFORM				ET_PLATFORM_WIN32
	#endif

#elif (TARGET_OS_IPHONE)

	#define ET_PLATFORM_IOS				1
	#define ET_PLATFORM					ET_PLATFORM_IOS
	#define CurrentPlatform				Platform_iOS

#elif (TARGET_OS_MAC)

	#define ET_PLATFORM_MAC				1
	#define ET_PLATFORM					ET_PLATFORM_MAC
	#define CurrentPlatform				Platform_Mac

#endif

#if (ET_PLATFORM_IOS || ET_PLATFORM_MAC)
	#include <string.h>
	#define ET_PLATFORM_APPLE	1
#else
	#define ET_PLATFORM_APPLE	0
#endif


#ifndef ET_PLATFORM
	#error Unable to determine current platform
#endif

#if defined(DEBUG) || defined(_DEBUG)
	#define ET_DEBUG	1
#else
	#define ET_DEBUG	0
#endif

namespace et
{
	typedef std::vector<std::string> StringList;

	enum Platform
	{
		Platform_Windows,
		Platform_iOS,
		Platform_Mac
	};

}