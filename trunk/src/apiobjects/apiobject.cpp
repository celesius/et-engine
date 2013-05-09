/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <time.h>
#include <et/core/tools.h>
#include <et/threading/atomiccounter.h>
#include <et/apiobjects/apiobject.h>

using namespace et;

static AtomicCounter apiObjectCounter;

std::string et::uniqueObjectName()
{ 
	std::string result;
	size_t unique = time(nullptr) + reinterpret_cast<size_t>(&result) % (rand() + 1) + rand();
	return "Obj-" + intToStr(apiObjectCounter.retain()) + "-" + intToStr(unique);
}
