/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	float queryTime();
	uint64_t queryTimeMSec();

	/**
	 * Returns device's screen size in native units.
	 * For Retina screens returns size in points
	 */
	vec2i nativeScreenSize();
	
	/**
	 * Reads application identifier from package
	 */
	std::string applicationIdentifierForCurrentProject();
	
	size_t streamSize(std::istream& s);
	
 	inline size_t roundToHighestPowerOfTow(size_t x)
	{
		x = x - 1;
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x + 1;
	}
	
	inline bool isPowerOfTwo(int x)
		{ return (x & (x - 1)) == 0; }
	
	inline bool isPowerOfTwo(size_t x)
		{ return (x & (x - 1)) == 0; }
}