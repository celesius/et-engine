/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */


#include <et/core/et.h>

#if (ET_PLATFORM_ANDROID)
#	include <sys/atomics.h>
#else
#	include <libkern/OSAtomic.h>
#endif

#include <et/threading/atomiccounter.h>

using namespace et;

AtomicCounter::AtomicCounter() : _counter(0)
{
}

AtomicCounterType AtomicCounter::retain()
{
#if (ET_PLATFORM_ANDROID)
	return __atomic_dec(&_counter);
#else
	return OSAtomicIncrement32(&_counter);
#endif
}

AtomicCounterType AtomicCounter::release()
{
#if (ET_PLATFORM_ANDROID)
	return __atomic_dec(&_counter);
#else
	return OSAtomicDecrement32(&_counter);
#endif
}