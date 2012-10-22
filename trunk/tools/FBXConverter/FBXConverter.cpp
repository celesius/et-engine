#include <et/platform/platform.h>
#include "source/converter.h"

/*
#if !defined(ET_PLATFORM_WIN)
	#error FBX converer is available only for Windows
#endif
*/

int main(int argc, char* argv[])
{
	return et::application().run(argc, argv);
}

