/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/et.h>

namespace et
{
#if (ET_PLATFORM_IOS || ET_PLATFORM_MAC)
	typedef int ReferenceCounterType;
#elif (ET_PLATFORM_WIN)
	typedef long ReferenceCounterType;
#else
	#error ReferenceCounterType is not defined
#endif
	
	class ReferenceCounter
	{
	public:
		ReferenceCounter();
		virtual ~ReferenceCounter() { }

		ReferenceCounterType retain();
		ReferenceCounterType release();

		ReferenceCounterType referenceCount()
			{ return _referenceCount; }

	private:
		volatile ReferenceCounterType _referenceCount;
	};
}