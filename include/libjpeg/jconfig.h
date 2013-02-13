#if defined(__MACH__) || defined(__ANDROID__)
#
#	include "jconfig.darwin.h"
#
#elif defined(_MSC_VER)
#
#	include "jconfig.win.h"
#
#else
#
#	error Unable to resolve libjpeg config
#
#endif
