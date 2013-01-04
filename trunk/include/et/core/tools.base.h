/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/platform/platform.h>
#include <et/core/constants.h>

namespace et
{
	float queryTime();
	
	inline int strToInt(const std::string& value)
		{ return atoi(value.c_str()); }
	
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
	
	float extractFloat(std::string& s);
}