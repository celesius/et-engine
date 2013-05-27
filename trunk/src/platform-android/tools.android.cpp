/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <et/core/tools.h>
#include <et/core/datastorage.h>
#include <et/platform-android/nativeactivity.h>

using namespace std;

static uint64_t startTime = 0;
static bool startTimeInitialized = false;

char et::pathDelimiter = '/';
char et::invalidPathDelimiter = '\\';


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
	JNIEnv* env = attachToThread();
	ANativeActivity* activity = et::sharedAndroidApplication()->activity;
	
    jmethodID methodID = env->GetMethodID(env->GetObjectClass(activity->clazz),
		"getPackageCodePath", "()Ljava/lang/String;");
	
    jobject codePath = env->CallObjectMethod(activity->clazz, methodID);
	
    jboolean isCopy = false;
    const char* cstr = env->GetStringUTFChars(reinterpret_cast<jstring>(codePath), &isCopy);
	assert(cstr);
	
	std::string result(cstr);
	
	if (isCopy)
		env->ReleaseStringUTFChars(reinterpret_cast<jstring>(codePath), cstr);
	
    return result;
}

std::string et::applicationTemporaryBaseFolder()
{
    return std::string();
}

std::string et::applicationDataFolder()
{
	return "assets/";
}

bool et::fileExists(const std::string& name)
{
	int index = -1;
	bool shouldCheckExternalFolder = false;
	zip* arch = et::sharedAndroidZipArchive();
	
	if (arch == nullptr)
	{
		shouldCheckExternalFolder = true;
	}
	else
	{
		index = zip_name_locate(arch, name.c_str(), 0);
		if (index == -1)
			shouldCheckExternalFolder = true;
	}
	
	if (shouldCheckExternalFolder)
	{
		if (access(name.c_str(), 0) == 0)
		{
			struct stat status = { };
			stat(name.c_str(), &status);
			return (status.st_mode & S_IFREG) == S_IFREG;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return index != -1;
	}
}

bool et::folderExists(const std::string& name)
{
	int index = -1;
	bool shouldCheckExternalFolder = false;
	zip* arch = et::sharedAndroidZipArchive();
	
	if (arch == nullptr)
	{
		shouldCheckExternalFolder = true;
	}
	else
	{
		index = zip_name_locate(arch, name.c_str(), 0);
		if (index == -1)
			shouldCheckExternalFolder = true;
	}
	
	if (shouldCheckExternalFolder)
	{
		if (access(name.c_str(), 0) == 0)
		{
			struct stat status = { };
			stat(name.c_str(), &status);
			return (status.st_mode & S_IFDIR) == S_IFDIR;
		}
		else
		{
			return false;
		}
	}
	else
	{
		struct zip_stat stat = { };
		zip_stat_init(&stat);
		zip_stat_index(arch, index, 0, &stat);
		return stat.size == 0;
	}
}

std::string et::applicationLibraryBaseFolder()
{
	return addTrailingSlash(std::string(et::sharedAndroidApplication()->activity->internalDataPath));
}

std::string et::applicationDocumentsBaseFolder()
{
	return addTrailingSlash(std::string(et::sharedAndroidApplication()->activity->internalDataPath));
}

bool et::createDirectory(const std::string& path, bool recursive)
{
	bool bSuccess = false;
    int nRC = ::mkdir( path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
    if (nRC == -1)
    {
        switch (errno)
        {
            case ENOENT:
			{
                if (et::createDirectory(path.substr(0, path.find_last_of(pathDelimiter)), true))
                    bSuccess = (::mkdir( path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
                break;
			}
				
            case EEXIST:
			{
                bSuccess = true;
                break;
			}
				
            default:
                break;
        }
    }
    else
	{
        bSuccess = true;
	}
	
	if (bSuccess)
		log::info("Directory %s created", path.c_str());
	
    return bSuccess;
}

bool et::removeFile(const std::string& path)
{
	return (::remove(path.c_str()) == 0);
}

bool et::removeDirectory(const std::string& path)
{
	return (::rmdir(path.c_str()) == 0);
}

void et::findFiles(const std::string& folder, const std::string& mask, bool/* recursive */,
	std::vector<std::string>& list)
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

et::vec2i et::nativeScreenSize()
{
	assert(0 && "Not supported yet");
	return vec2i(0);
}

unsigned long et::getFileDate(const std::string& path)
{
	struct stat s = { };
	stat(path.c_str(), &s);
	return static_cast<unsigned long>(s.st_mtime_nsec / 1000000);
}

void et::getFolderContent(const std::string& path, std::vector<std::string>& aList)
{
	log::info("Retreiving content of the %s", path.c_str());
	
	DIR* dir = opendir(path.c_str());
	if (dir == nullptr)
	{
		zip* arch = et::sharedAndroidZipArchive();
		if (arch == nullptr)
		{
			log::error("Unable to get contents of the %s", path.c_str());
		}
		else
		{
			int numFiles = zip_get_num_files(arch);
			for (int i = 0; i < numFiles; ++i)
			{
				std::string aName(zip_get_name(arch, i, 0));
				if (aName.find(path) == 0)
				{
					log::info("Got file %s", aName.c_str());
				}
			}
		}
	}
	else
	{
		struct dirent* ent = nullptr;
		
		do
		{
			ent = readdir(dir);
			if (ent)
			{
				log::info("%s\n", ent->d_name);
				aList.push_back(std::string(ent->d_name));
			}
		}
		while (ent != nullptr);
		
		closedir (dir);
	}
}