/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <time.h>
#include <et/core/tools.h>
#include <et/apiobjects/apiobject.h>

using namespace et;

static size_t apiObjectCounter = 0;

std::string et::APIObjectData::uniqueName()
{ 
	std::string result;
	size_t unique = time(0) % (rand() + 1) + reinterpret_cast<size_t>(&result) % (rand() + 1) + rand();
	return "Obj-" + intToStr(++apiObjectCounter) + "-" + intToStr(unique);
}
