/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#include <fstream>
#include <et/core/tools.h>
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

	size_t i = 0;
	bool inQuote = false;
	while ((i < raw.size()) && raw[i])
	{
		if (raw[i] == CommentChar)
		{
			inQuote = false;
			i = parseComment(raw, i+1);
		}
		else 
		{
			if (raw[i] == KeyChar)
				inQuote = !inQuote;

			if (inQuote || !isWhitespaceChar(raw[i]))
				keyValues.push_back(raw[i]);
			
			++i;
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
	
	i = 0;
	while (source[i] && (i < source.size()) && source[i+1])
		i = (source[i] == KeyChar) ? parseKey(source, i+1) : ++i;
}


size_t Locale::parseKey(const StringDataStorage& data, size_t index)
{
	size_t keyEnd = 0;
	size_t valueStart = 0;
	size_t i = index;
	
	while ((i < data.size()) && data[i] && (data[i] != ';'))
	{
		if (data[i] == KeyChar)
		{
			if (keyEnd == 0)
				keyEnd = i;
			
			else if (valueStart == 0)
				valueStart = i+1;
		}
		++i;
	}

	size_t keyLenght = keyEnd - index;
	size_t valueLenght = i - valueStart - 1;
	
	std::string key(keyLenght, 0);
	for (size_t i = 0; i < keyLenght; ++i)
		key[i] = data[index+i];
	
	std::string value(valueLenght, 0);
	for (size_t i = 0; i < valueLenght; ++i)
		value[i] = data[valueStart+i];

	_localeMap[key] = value;
	
	return i+1;
}

size_t Locale::parseComment(const StringDataStorage& data, size_t index)
{
	if (data[index] == CommentChar) // line comment
	{
		while ((index < data.size()) && data[index] && !((data[index] == 0x0a)||(data[index] == 0x0d)))
			++index;
		return index + 1;
	}
	else if (data[index] == '*') // block comment
	{
		while ((index + 1 < data.size()) && data[index] && !((data[index] == '*') && (data[index+1] == CommentChar)) )
			++index;
		
		return index + 2;
	}
	else
	{
		std::cout << "Unsupported comment format in language file";
		return index;
	}
}

void Locale::printKeyValues()
{
	std::cout << std::endl << "Key value pairs." << std::endl;
	for (LocaleMap::iterator i = _localeMap.begin(), e = _localeMap.end(); i != e; ++i)
	{
		const std::string& key = i->first;
		std::string& value = i->second;
		std::cout << key.size() << " / " << value.size() << " -> " << key << " -> " << value << std::endl;
		std::cout.flush();
	}
}