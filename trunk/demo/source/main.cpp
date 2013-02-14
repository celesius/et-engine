#include <et/core/et.h>
#include "maincontroller.h"

#if (ET_PLATFORM_ANDROID)
#	include <android_native_app_glue.h>
#endif

int main(int argc, char* argv[])
{
	return et::application().run(argc, argv);
}

void android_main(struct android_app* state)
{
	app_dummy();
	main(0, 0);
}
