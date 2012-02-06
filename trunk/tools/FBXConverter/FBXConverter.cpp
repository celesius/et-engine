#include <et/platform/platform.h>

#if !defined(ET_PLATFORM_WIN)
	#error FBX converer is available only for Windows
#endif

#include "source/converter.h"

et::IApplicationDelegate* et::Application::initApplicationDelegate()
{
	return new fbxc::Converter();
}

int main(int argc, char* argv[])
{
	return et::application().run(argc, argv);
}

