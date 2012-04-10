#include <time.h>
#include <et/apiobjects/apiobject.h>
#include <et/core/tools.h>

using namespace et;

static size_t apiObjectCounter = 0;

std::string et::APIObjectData::uniqueName()
{ 
	std::string result;
	size_t unique = time(0) % (rand() + 1) + reinterpret_cast<size_t>(&result) % (rand() + 1) + rand();
	return "Obj-" + intToStr(++apiObjectCounter) + "-" + intToStr(unique);
}