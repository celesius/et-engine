/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <sys/time.h>
#include <et/core/datastorage.h>
#include <et/core/tools.h>

using namespace std;

static uint64_t startTime = 0;
static bool startTimeInitialized = false;

char et::pathDelimiter = '/';

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
    return std::string();
}

std::string et::applicationDataFolder()
{
	return std::string();
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
    return false;
}

bool et::folderExists(const std::string& name)
{
	return false;
}

std::string et::applicationLibraryBaseFolder()
{
	return std::string();
}

std::string et::applicationDocumentsBaseFolder()
{
	return std::string();
}

void et::createDirectory(const std::string& name)
{
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
