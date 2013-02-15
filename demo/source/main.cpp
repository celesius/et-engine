#include <et/core/et.h>
#include "maincontroller.h"

int main(int argc, char* argv[])
{
	return et::application().run(argc, argv);
}

#if (ET_PLATFORM_ANDROID)

#	include <android_native_app_glue.h>

void android_main(struct android_app* state)
{
	app_dummy();
	main(0, 0);
}
#endif
