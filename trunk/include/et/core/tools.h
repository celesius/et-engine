#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <et/core/constants.h>
#include <et/geometry/geometry.h>

#pragma once

namespace et
{
	typedef std::vector<std::string> StringList;

	// platform-specific code
	extern char pathDelimiter;
	float queryTime();
	std::string applicationPath();
	std::string applicationDataFolder();
	std::string applicationDocumentsFolder();
	std::string normalizeFilePath(std::string s);
	bool fileExists(const std::string& name);
	bool folderExists(const std::string& name);
	void createDirectory(const std::string& name);
	void findFiles(const std::string& folder, const std::string& mask, bool recursive, StringList& list);
	void findSubfolders(const std::string& folder, bool recursive, StringList& list);

	// common code
	inline bool isWhitespaceChar(char c)
	{
		return (c == 32) || (c == 13) || (c == 10) || (c == 9);
	}

	inline void lowercase(std::string& str)
	{
		for (size_t i = 0; i < str.length(); i++)
			str[i] = static_cast<char>(tolower(str[i]));
	}

	inline int strToInt(const std::string& value)
	{
		return atoi(value.c_str());
	}

	template <typename i_type>
	inline std::string intToStr(i_type value);
	
	template <>
	inline std::string intToStr<int>(int value)
	{ 
		char buffer[32] = { };
		sprintf(buffer, "%d", value);
		return buffer;
	}

	template <>
	inline std::string intToStr<unsigned long>(unsigned long value)
	{ 
		char buffer[32] = { };
		sprintf(buffer, "%lu", value);
		return buffer;
	}
	
	template <>
	inline std::string intToStr<unsigned int>(unsigned int value)
	{ 
		char buffer[32] = { };
		sprintf(buffer, "%u", value);
		return buffer;
	}	

	inline size_t streamSize(std::istream& s)
	{
		std::streamoff currentPos = s.tellg();

		s.seekg(0, std::ios::end);
		std::streamoff endPos = s.tellg();
		s.seekg(currentPos, std::ios::beg);

		return static_cast<size_t>(endPos);
	}

	inline size_t roundToHighestPowerOfTow(size_t x)
	{
		x = x - 1;
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x + 1;
	}

	template <typename T>
	inline T intPower(T value, int power)
	{
		if (power == 0) return 1;

		T result = static_cast<T>(1);
		for (int i = 1; i <= power; ++i)
			result *= value; 

		return result;
	}

	std::string getFilePath(const std::string& name);
	std::string getFileName(const std::string& fullPath);
	std::string removeUpDir(std::string name);
	std::string getFileExt(std::string name);
	std::string& trim(std::string &str);
	std::string floatToStr(float value, int precission = 5);
	std::string floatToTimeStr(float value, bool showMSec = true);
	std::string loadTextFile(const std::string& fileName);
	std::string addTrailingSlash(const std::string& path);
	std::string replaceFileExt(const std::string& fileName, const std::string& newExt);
	std::string removeFileExt(const std::string& fileName);

	float extractFloat(std::string& s);

	vec4 strToVec4(const std::string& s, const std::string& delimiter = ";");
	vec4 strToVec4(std::string& s, const std::string& delimiter = ";");
	vec4 strHexToVec4(const std::string& s);
}