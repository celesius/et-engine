/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#include <fstream>
#include <et/core/tools.h>
#include <et/core/containers.h>
#include <et/locale/locale.h>

using namespace et;

const char CommentChar = '/';
const char KeyChar = '\"';

std::string Locale::localeLanguage(size_t locale)
{
	char lang[3] = { locale & 0x000000ff, (locale & 0x0000ff00) >> 8, 0 };
	return std::string(lang);
}

bool Locale::loadLanguageFile(const std::string& fileName)
{
	if (fileExists(fileName))
	{
		parseLanguageFile(fileName);
		return true;
	}

	std::cout << "Unable to load language file `" << fileName << "`" << std::endl;
	return false;
}

bool Locale::loadCurrentLanguageFile(const std::string& rootFolder, const std::string& extension)
{
	std::string lang = Locale::localeLanguage(Locale::currentLocale());
	std::string fileName = addTrailingSlash(rootFolder) + lang + extension;
	if (fileExists(fileName))
	{
		parseLanguageFile(fileName);
		return true;
	}
	
	std::cout << "Unable to locate language file `" << fileName << "` in folder " << rootFolder << std::endl;
	return false;
}

std::string Locale::localizedString(const std::string& key)
{
	LocaleMap::iterator i = _localeMap.find(key);
	return (i == _localeMap.end()) ? key : i->second;
}

void Locale::parseLanguageFile(const std::string& fileName)
{
	_localeMap.clear();

	std::ifstream file(fileName.c_str(), static_cast<std::ios_base::openmode>(std::ios_base::beg | std::ios_base::binary));
	if (file.fail()) return;

	StringDataStorage raw(streamSize(file) + 1, 0);
	file.read(raw.data(), raw.size());

	StringDataStorage keyValues(raw.size(), 0);

	bool inQuote = false;
	char* ptr = raw.data();
	while (*ptr)
	{
		if (*ptr == CommentChar)
		{
			inQuote = false;
			ptr = parseComment(ptr + 1);
		}
		else 
		{
			if (*ptr == KeyChar)
				inQuote = !inQuote;

			if (inQuote || !isWhitespaceChar(*ptr))
				keyValues.push_back(*ptr);

			++ptr;
		}
	}

	inQuote = false;
	size_t sourceLength = keyValues.currentIndex();
	StringDataStorage source(sourceLength + 1, 0);
	for (size_t i = 0; i < sourceLength; ++i)
	{
		char c = keyValues[i];

		if (c == KeyChar) 
			inQuote = !inQuote;

		bool isWhiteSpace = !inQuote && ((c == 0x20) || (c == 0x09));
		bool isNewLine = (c == 0x0a) || (c == 0x0d);
		bool hasNextQuoteMark = (i + 1 < sourceLength) && (keyValues[i+1] == KeyChar);
		bool hasPrevQuoteMark = (i > 0) && (keyValues[i-1] == KeyChar);
		bool shouldConcatMultiline = ((c == KeyChar) && hasNextQuoteMark) || ((c == KeyChar) && hasPrevQuoteMark);
		if (isWhiteSpace || isNewLine || shouldConcatMultiline) continue;

		source.push_back(c);
	}

	ptr = source.data();
	while (*ptr && (*ptr+1))
		ptr = (*ptr == KeyChar) ? parseKey(ptr+1) : ptr + 1;
}


char* Locale::parseComment(char* ptr)
{
	if (*ptr == '/') // line comment
	{
		while ((*ptr) && !((*ptr == 0x0a) || (*ptr == 0x0d)))
		{
			++ptr;
		}
		return ptr + 1;
	}
	else if (*ptr == '*') // block comment
	{
		while ( (*ptr) && (*(ptr+1)) && !( (*ptr == '*') && ( (*(ptr+1)) == '/')) ) ++ptr;
		return ptr + 2;
	}
	else
	{
		std::cout << "Unsupported comment format in language file";
		return ptr + 1;
	}
}

char* Locale::parseKey(char* ptr)
{
	char* ptr0 = ptr;
	char* keyEnd = 0;
	char* valueStart = 0;
	while (*ptr && (*ptr != ';'))
	{
		if (*ptr == KeyChar)
		{
			if (keyEnd == 0)
				keyEnd = ptr;
			else if (valueStart == 0)
				valueStart = ptr + 1;
		}
		++ptr;
	}

	StringDataStorage key(keyEnd - ptr0 + 1, 0);
	memcpy(key.data(), ptr0, key.size() - 1);

	StringDataStorage value(ptr - valueStart, 0);
	memcpy(value.data(), valueStart, value.size() - 1);

	_localeMap[std::string(key.data())] = std::string(value.data());

	return ptr + 1;
}
