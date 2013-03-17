/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/et.h>

namespace et
{
#if (ET_PLATFORM_IOS || ET_PLATFORM_MAC || ET_PLATFORM_ANDROID)
#
	typedef int AtomicCounterType;
#
#elif (ET_PLATFORM_WIN)
#
	typedef long AtomicCounterType;
#
#else
#
#	error AtomicCounterType is not defined
#
#endif
	
	class AtomicCounter
	{
	public:
		AtomicCounter();
		virtual ~AtomicCounter() { }

		AtomicCounterType retain();
		AtomicCounterType release();

		volatile const AtomicCounterType& atomicCounterValue() const
			{ return _counter; }

	private:
		volatile AtomicCounterType _counter;
	};
}