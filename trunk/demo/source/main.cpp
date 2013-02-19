#include <et/core/et.h>
#include "maincontroller.h"

int main(int argc, char* argv[])
{
	return et::application().run(argc, argv);
}

#if (ET_PLATFORM_ANDROID)

#include <et/platform-android/nativeactivity.h>

void android_main(android_app* state)
{
	et::initSharedAndroidApplication(state);
	main(0, 0);
}

#endif

