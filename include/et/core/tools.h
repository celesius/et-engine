/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/geometry/geometry.h>

namespace et
{
	// platform-specific code
	extern char pathDelimiter;
	extern char invalidPathDelimiter;
	
	float queryTime();
	uint64_t queryTimeMSec();
	
	std::string applicationPath();
	std::string applicationPackagePath();
	std::string applicationDataFolder();
	
	std::string applicationLibraryBaseFolder();
	std::string applicationTemporaryBaseFolder();
	std::string applicationDocumentsBaseFolder();
	
	std::string normalizeFilePath(std::string s);
	
	bool fileExists(const std::string& name);
	bool folderExists(const std::string& name);
	
	bool createDirectory(const std::string& name, bool intermediates);
	bool removeDirectory(const std::string& name);
	bool removeFile(const std::string& name);
	
	void getFolderContent(const std::string& folder, StringList& list);
	void findFiles(const std::string& folder, const std::string& mask, bool recursive, StringList& list);
	void findSubfolders(const std::string& folder, bool recursive, StringList& list);
	void openUrl(const std::string& url);

	unsigned long getFileDate(const std::string& path);
	
	std::string unicodeToUtf8(const std::wstring& w);
	std::wstring utf8ToUnicode(const std::string& mbcs);
	
	std::string applicationIdentifierForCurrentProject();

	/**
	 * Returns device's screen size in native units.
	 * For Retina screens returns size in points
	 */
	vec2i nativeScreenSize();
	
	/*
	 * Common code
	 */
	inline std::string lowercase(const std::string& s)
	{
		std::string str(s);
		ET_ITERATE(str, char&, c, c = ::tolower(c) & 0xff);
		return str;
	}
	
	inline void lowercase(std::string& s)
	{
		ET_ITERATE(s, char&, c, c = ::tolower(c) & 0xff);
	}

	inline std::string uppercase(const std::string& s)
	{
		std::string str(s);
		ET_ITERATE(str, char&, c, c = ::toupper(c) & 0xff);
		return str;
	}

	inline void uppercase(std::string& s)
	{
		ET_ITERATE(s, char&, c, c = ::toupper(c) & 0xff);
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
		stream << value.x << ET_CSV_DELIMITER << value.y << ET_CSV_DELIMITER <<
			value.z << ET_CSV_DELIMITER << value.w;
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
	
	template <typename T>
	inline std::ostream& operator << (std::ostream& stream, const Rect<T>& value)
	{
		stream << value.left << ", " << value.top << ", " << value.width << "x" << value.height;
		return stream;
	}
		
	inline std::string intToStr(int value)
	{
		char buffer[32] = { };
		sprintf(buffer, "%d", value);
		return buffer;
	}

	inline std::string intToStr(unsigned long value)
	{
		char buffer[32] = { };
		sprintf(buffer, "%lu", value);
		return buffer;
	}
	
	inline std::string intToStr(unsigned int value)
	{
		char buffer[32] = { };
		sprintf(buffer, "%u", value);
		return buffer;
	}

	inline std::string intToStr(void* value)
	{
		char buffer[32] = { };
		sprintf(buffer, "%08zX", reinterpret_cast<size_t>(value));
		return buffer;
	}

	inline std::string intToStr(const void* value)
	{
		char buffer[32] = { };
		sprintf(buffer, "%08zX", reinterpret_cast<size_t>(value));
		return buffer;
	}

	inline int strToInt(const std::string& value)
		{ return std::atoi(value.c_str()); }

	inline float strToDouble(const std::string& value)
		{ return std::atof(value.c_str()); }

	inline float strToFloat(const std::string& value)
		{ return static_cast<float>(strToDouble(value)); }

	inline bool strToBool(std::string s)
	{
		lowercase(s);
		return (s == "true") || (s == "1");
	}


	inline bool isPowerOfTwo(int value)
		{ return (value & (value - 1)) == 0; }
	
	inline bool isNewLineChar(char c)
		{ return (c == ET_RETURN) || (c == ET_NEWLINE); }
	
	inline bool isNewLineChar(wchar_t c)
		{ return (c == ET_RETURN) || (c == ET_NEWLINE); }
	
	inline bool isWhitespaceChar(char c)
		{ return (c == ET_SPACE) || (c == ET_RETURN) || (c == ET_NEWLINE) || (c == ET_TAB); }
	
	inline bool isWhitespaceChar(wchar_t c)
		{ return (c == ET_SPACE) || (c == ET_RETURN) || (c == ET_NEWLINE) || (c == ET_TAB); }
	
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

	std::string removeWhitespace(const std::string&);
	
	float extractFloat(std::string& s);
	
	vec4 strToVec4(const std::string& s, const std::string& delimiter = ";");
	vec4 strToVec4(std::string& s, const std::string& delimiter = ";");
	
	vec4 strHexToVec4(const std::string& s);
	vec4 strHexToVec4(const std::wstring& s);
}