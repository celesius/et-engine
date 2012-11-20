/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <fstream>
#include <et/core/constants.h>
#include <et/core/debug.h>
#include <et/geometry/geometry.h>

namespace et
{
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
	std::string selectFile(const StringList& mask);
	std::string selectFile(const std::string& description, const std::string& ext);
	void openUrl(const std::string& url);
	
	std::string unicodeToUtf8(const std::wstring& w);
	std::wstring utf8ToUnicode(const std::string& mbcs);
	
	std::string applicationIdentifierForCurrentProject();

	/*
	 * Common code
	 */
	inline bool isNewLineChar(char c)
		{ return (c == 13) || (c == 10); }
	inline bool isWhitespaceChar(char c)
		{ return (c == 32) || (c == 13) || (c == 10) || (c == 9); }
	inline bool isWhitespaceChar(wchar_t c)
		{ return (c == 32) || (c == 13) || (c == 10) || (c == 9); }
	inline bool isNewLineChar(wchar_t c)
		{ return (c == 13) || (c == 10); }

	inline void lowercase(std::string& str)
	{
		for (size_t i = 0; i < str.length(); i++)
			str[i] = static_cast<char>(tolower(str[i]));
	}

	inline int strToInt(const std::string& value)
		{ return atoi(value.c_str()); }

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
	
	inline bool isPowerOfTwo(int value)
		{ return (value & (value - 1)) == 0; }

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

	template <typename T>
	inline std::ostream& operator << (std::ostream& stream, const vector2<T>& value) 
	{ 
		stream << value.x << ET_CSV_DELIMITER << value.y;
		return stream;
	}

	template <typename T>
	inline std::ostream& operator << (std::ostream& stream, const vector3<T>& value) 
	{ 
		stream << value.x << ET_CSV_DELIMITER << value.y << ET_CSV_DELIMITER << value.z;
		return stream;
	}

	template <typename T>
	inline std::ostream& operator << (std::ostream& stream, const vector4<T>& value) 
	{ 
		stream << value.x << ET_CSV_DELIMITER << value.y << ET_CSV_DELIMITER << value.z << ET_CSV_DELIMITER << value.w;
		return stream;
	}


	template <typename T>
	inline std::ostream& operator << (std::ostream& stream, const matrix4<T>& value) 
	{ 
		stream << "{" << std::endl << "\t" << value[0] << std::endl << 
			"\t" << value[1] << std::endl << "\t" << value[2] << std::endl << 
			"\t" << value[3] << std::endl << "}" << std::endl;
		return stream;
	}
		
	std::ostream& operator << (std::ostream& stream, const StringList& list);
		
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
	vec4 strHexToVec4(const std::wstring& s);
}