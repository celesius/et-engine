#pragma once

#include <android/log.h>
#include <android_native_app_glue.h>

#define androidLog(...) { __android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__); }
