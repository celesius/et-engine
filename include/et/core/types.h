#pragma once 

#include <et/platform/platform.h>

namespace et
{
	typedef unsigned int IndexType;
	typedef unsigned short ShortIndexType;
	typedef unsigned char SmallIndexType;

	static const IndexType InvalidIndex = static_cast<IndexType>(-1);
}