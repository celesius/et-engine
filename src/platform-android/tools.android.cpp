/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <jni.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <libzip/zip.h>

#include <et/core/tools.h>
#include <et/core/datastorage.h>

#include <et/platform-android/nativeactivity.h>

using namespace std;

static uint64_t startTime = 0;
static bool startTimeInitialized = false;

char et::pathDelimiter = '/';

namespace et
{
	extern android_app* sharedAndroidApplication();
	extern zip* sharedAndroidZipArchive();
}

uint64_t queryActualTime()
{
	timeval tv = { };
	gettimeofday(&tv, 0);
	
	return static_cast<uint64_t>(tv.tv_sec) * 1000 +
			static_cast<uint64_t>(tv.tv_usec) / 1000;
}

float et::queryTime()
{
	return static_cast<float>(queryTimeMSec()) / 1000.0f;
}

uint64_t et::queryTimeMSec()
{
	if (!startTimeInitialized)
	{
		startTime = queryActualTime();
		startTimeInitialized = true;
	};
	
	return queryActualTime() - startTime;
}

std::string et::applicationPath()
{
    return et::applicationPackagePath() + "/";
}

std::string et::applicationPackagePath()
{
    JNIEnv* env = et::sharedAndroidApplication()->activity->env;
	ANativeActivity* activity = et::sharedAndroidApplication()->activity;

	activity->vm->AttachCurrentThread(&env, nullptr);

    jmethodID methodID =
		env->GetMethodID(env->GetObjectClass(activity->clazz), "getPackageCodePath", "()Ljava/lang/String;");

    jobject result = env->CallObjectMethod(activity->clazz, methodID);

    jboolean isCopy = false;
    const char* str = env->GetStringUTFChars(reinterpret_cast<jstring>(result), &isCopy);

	et::log::info("Application package path: ", str);

    return std::string(str);
}

std::string et::applicationDataFolder()
{
	return "assets/";
}

std::string et::normalizeFilePath(string s)
{
	for (auto& c : s)
	{
		if (c == '\\')
			c = pathDelimiter;
	}
	
	return s;
}

bool et::fileExists(const std::string& name)
{
	zip* arch = et::sharedAndroidZipArchive();
	return (arch != nullptr) && (zip_name_locate(arch, name.c_str(), 0) != -1);
}

bool et::folderExists(const std::string& name)
{
	zip* arch = et::sharedAndroidZipArchive();
	if (arch == nullptr)
	{
		log::info("looking for folder without archive");
		return false;
	}

	int index = zip_name_locate(arch, name.c_str(), 0);
	if (index == -1) return false;

	struct zip_stat stat = { };
	zip_stat_init(&stat);
	zip_stat_index(arch, index, 0, &stat);
	et::log::info("folder stat: size %u", stat.size);
	return stat.size == 0;
}

std::string et::applicationLibraryBaseFolder()
{
	return addTrailingSlash(std::string(et::sharedAndroidApplication()->activity->internalDataPath));
}

std::string et::applicationDocumentsBaseFolder()
{
	return addTrailingSlash(std::string(et::sharedAndroidApplication()->activity->internalDataPath));
}

void et::createDirectory(const std::string& name)
{
	int value = mkdir(name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	
	if (value)
		log::error("attempting to create a directory: %s ended up with %d", name.c_str(), value);
}

void et::findFiles(const std::string& folder, const std::string& mask, bool /* recursive */, std::vector<std::string>& list)
{
}

void et::findSubfolders(const std::string& folder, bool recursive, std::vector<std::string>& list)
{
}

void et::openUrl(const std::string& url)
{
}

std::string et::unicodeToUtf8(const std::wstring& w)
{
	return std::string();
}

std::wstring et::utf8ToUnicode(const std::string& mbcs)
{
	return std::wstring();
}

std::string et::applicationIdentifierForCurrentProject()
{
	return std::string();
}
