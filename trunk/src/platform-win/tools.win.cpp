/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <et/core/tools.h>
#include <et/core/containers.h>

static bool shouldInitializeFrequency = true;
static double performanceFrequency = 0.0;

void initPerformanceFrequency()
{
	__int64 freq = 0;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
	performanceFrequency = 1.0 / static_cast<double>(freq);
	shouldInitializeFrequency = false;
}

float et::queryTime()
{
	if (shouldInitializeFrequency)
		initPerformanceFrequency();

	__int64 counter = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counter));
	return static_cast<float>(static_cast<double>(counter) * performanceFrequency);
} 

char et::pathDelimiter = '\\';

std::string et::applicationPath()
{
	char ExePath[MAX_PATH] = { };
	GetModuleFileNameA(0, ExePath, MAX_PATH);
	return getFilePath(normalizeFilePath(ExePath));
}

std::string et::normalizeFilePath(std::string s)
{
	for (std::string::iterator i = s.begin(), e = s.end(); i != e; ++i)
	{
		if ((*i) == '/')
			(*i) = pathDelimiter;
	}

	return s;
}

bool et::fileExists(const std::string& name)
{
	return GetFileAttributes(name.c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool et::folderExists(const std::string& folder)
{
	return GetFileAttributes(folder.c_str()) == FILE_ATTRIBUTE_DIRECTORY;
}

void et::findFiles(const std::string& folder, const std::string& mask, bool recursive, StringList& list)
{
	std::string normalizedFolder = addTrailingSlash(folder);
	std::string searchPath = normalizedFolder + mask;

	StringList folderList;
	if (recursive)
	{
		std::string foldersSearchPath = normalizedFolder + "*.*";
		WIN32_FIND_DATA folders = { };
		HANDLE folderSearch = FindFirstFile(foldersSearchPath.c_str(), &folders);
		if (folderSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(folders.cFileName, ".") && strcmp(folders.cFileName, "..") && 
					((folders.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
				{
					std::string folderName(folders.cFileName);
					folderList.push_back(normalizedFolder + folderName);
				}
			}
			while (FindNextFile(folderSearch, &folders));
			FindClose(folderSearch);
		}
	}

	WIN32_FIND_DATA data = { };
	HANDLE search = FindFirstFile(searchPath.c_str(), &data);

	if (search != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, ".."))
			{
				std::string fileName(data.cFileName);
				list.push_back(normalizedFolder + fileName);
			}
		}
		while (FindNextFile(search, &data));
		FindClose(search);
	}

	if (recursive)
	{
		for (StringList::iterator i = folderList.begin(), e = folderList.end(); i != e; ++i)
			findFiles(*i, mask, recursive, list);
	}
}

std::string et::applicationDataFolder()
{
	std::string appPath = applicationPath();

	std::string value = appPath + "..\\data\\"; 
	if (et::fileExists(value))
		return value;

	value = appPath + "..\\..\\data\\"; 
	if (et::fileExists(value))
		return value;

	return appPath + "data\\";
}

std::string et::applicationDocumentsFolder()
{
	return applicationDataFolder();
}

void et::createDirectory(const std::string& name)
{
	CreateDirectory(name.c_str(), 0);
}

void et::findSubfolders(const std::string& folder, bool recursive, StringList& list)
{
	std::string normalizedFolder = addTrailingSlash(folder);
	std::string foldersSearchPath = normalizedFolder + "*.*";
	StringList folderList;

	WIN32_FIND_DATA folders = { };
	HANDLE folderSearch = FindFirstFile(foldersSearchPath.c_str(), &folders);
	if (folderSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(folders.cFileName, ".") && strcmp(folders.cFileName, "..") && 
				((folders.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string folderName(folders.cFileName);
				folderList.push_back(normalizedFolder + folderName + "\\");
			}
		}
		while (FindNextFile(folderSearch, &folders));
		FindClose(folderSearch);
	}

	if (recursive)
	{
		for (StringList::iterator i = folderList.begin(), e = folderList.end(); i != e; ++i)
			findSubfolders(*i, true, list);
	}

	list.insert(list.end(), folderList.begin(), folderList.end());
}

std::string et::selectFile(const StringList& mask)
{
	size_t maskLen = 1;
	for (StringList::const_iterator i = mask.begin(), e = mask.end(); i != e; ++i)
		maskLen += i->size() + 1;

	char result[MAX_PATH] = { };
	char* filter = new char[maskLen];
	memset(filter, 0, maskLen);

	size_t offset = 0;
	for (StringList::const_iterator i = mask.begin(), e = mask.end(); i != e; ++i)
	{
		memcpy(filter + offset, i->data(), i->size());
		offset += i->size() + 1;
	}

	OPENFILENAME ofn = { };
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter = filter;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.lpstrFile = result;
	ofn.nMaxFile = MAX_PATH;

	GetOpenFileName(&ofn);

	delete [] filter;
	return std::string(result);
}

std::string et::selectFile(const std::string& description, const std::string& ext)
{
	StringList m;
	m.push_back(description);
	m.push_back(ext);
	return selectFile(m);
}

void et::openUrl(const std::string& url)
{
	ShellExecute(0, "open", url.c_str(), 0, 0, SW_SHOWNORMAL);
}

std::string et::unicodeToUtf8(const std::wstring& w)
{
	int mbcWidth = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, 0, 0, 0, 0);

	if (mbcWidth == 0)
		return std::string();

	DataStorage<char> result(mbcWidth + 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, result.data(), result.size(), 0, 0);

	return std::string(result.data());
}

std::wstring et::utf8ToUnicode(const std::string& mbcs)
{
	int uWidth = MultiByteToWideChar(CP_UTF8, 0, mbcs.c_str(), -1, 0, 0);
	if (uWidth == 0)
	{
		switch (GetLastError())
		{
		case ERROR_INSUFFICIENT_BUFFER:
			std::cout << "A supplied buffer size was not large enough, or it was incorrectly set to NULL." << std::endl;
			break;
		case ERROR_INVALID_FLAGS:
			std::cout << "The values supplied for flags were not valid." << std::endl;
			break;
		case ERROR_INVALID_PARAMETER:
			std::cout << "Any of the parameter values was invalid." << std::endl;
			break;
		case ERROR_NO_UNICODE_TRANSLATION:
			std::cout << "Invalid Unicode was found in a string" << std::endl;
			break;
		default:
			std::cout << "Failed to convert utf-8 to wchar_t" << std::endl;
		}

		return std::wstring();
	}

	DataStorage<wchar_t> result(uWidth + 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, mbcs.c_str(), -1, result.data(), result.size());

	return std::wstring(result.data());
}
