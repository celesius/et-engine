/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <et/threading/atomiccounter.h>

using namespace et;

AtomicCounter::AtomicCounter() : _counter(0)
	{ }

AtomicCounterType AtomicCounter::retain()
	{ return InterlockedIncrement(&_counter); }

AtomicCounterType AtomicCounter::release()
	{ return InterlockedDecrement(&_counter); }