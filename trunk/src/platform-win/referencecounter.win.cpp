/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <et/threading/Referencecounter.h>

using namespace et;

ReferenceCounter::ReferenceCounter() : _referenceCount(0)
	{ }

ReferenceCounterType ReferenceCounter::retain()
	{ return InterlockedIncrement(&_referenceCount); }

ReferenceCounterType ReferenceCounter::release()
	{ return InterlockedDecrement(&_referenceCount); }