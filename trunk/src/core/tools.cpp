/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/datastorage.h>
#include <et/core/stream.h>
#include <et/core/tools.h>

namespace et
{
	std::string floatToTimeStr(float value, bool showMSec)
	{
		int seconds = static_cast<int>(value);
		int mSec = static_cast<int>((value - static_cast<float>(seconds)) * 1000.0f);

		int hours = seconds / 3600;
		seconds -= 3600 * hours;
		int minutes = seconds / 60;
		seconds -= minutes * 60;
        
        hours = abs(hours);
        minutes = abs(minutes);
        seconds = abs(seconds);
        mSec = abs(mSec);
		std::string sHours = intToStr(hours);
		std::string sMin = intToStr(minutes);
		std::string sSec = intToStr(seconds);
		std::string sMSec = intToStr(mSec);

		if ((hours > 0) && (minutes < 10))
			sMin = "0" + sMin;
		
		if (seconds < 10)
			sSec = "0" + sSec;

		if (showMSec)
		{
			if (mSec < 100)
				sMSec = "0" + sMSec;
			if (mSec < 10)
				sMSec = "0" + sMSec;
		}

		std::string result;

		if (hours > 0)
			result = sHours + ":" + sMin + ":" + sSec;
		else
			result = sMin + ":" + sSec;

		if (showMSec)
			result += ":" + sMSec;
        
        if (value < 0)
            result = "-" + result;

		return result;
	}

	std::string floatToStr(float v, int precission)
	{ 
		char format[16] = { };
		sprintf(format, "%%.%df", precission);
        
		char buffer[256] = { };
		sprintf(buffer, format, v);
        
		return std::string(buffer);
	}

	std::string loadTextFile(const std::string& fileName)
	{
		InputStream file(fileName, StreamMode_Binary);
		if (file.invalid()) return std::string();

		StringDataStorage data(streamSize(file.stream()) + 1, 0);
		file.stream().read(data.data(), static_cast<std::streamsize>(data.size()));
		
		return std::string(data.data());
	}

	std::string addTrailingSlash(const std::string& path)
	{
		if (path.empty()) return path;

		std::string::value_type t = *path.rbegin();
		return ((t == invalidPathDelimiter) || (t == pathDelimiter)) ? path : path + pathDelimiter;
	}

	std::string replaceFileExt(const std::string& fileName, const std::string& newExt)
	{
		std::string name = getFileName(fileName);
		std::string path = getFilePath(fileName);

		size_t dotPos = name.find_last_of(".");
		if (dotPos == std::string::npos)
			return fileName + newExt;

		name.erase(dotPos);
		return path + name + newExt;
	}

	std::string removeFileExt(const std::string& fileName)
	{
		std::string name = getFileName(fileName);
		std::string path = getFilePath(fileName);

		size_t dotPos = name.find_last_of(".");
		if (dotPos == std::string::npos)
			return fileName;

		name.erase(dotPos, name.length() - dotPos);
		return path + name;
	}

	std::string& trim(std::string &str)
	{
		size_t strSize = str.length();
		if (!strSize) return str;

		size_t leadingWhitespace = 0;
		size_t trailingWhitespace = 0;
		size_t pos = 0;
		while ((pos < strSize) && isWhitespaceChar(str[pos++])) 
			++leadingWhitespace;

		pos = strSize - 1;
		while ((pos > 0) && isWhitespaceChar(str[pos--])) 
			++trailingWhitespace;

		if (leadingWhitespace)
			str.erase(0, leadingWhitespace);

		if (trailingWhitespace)
			str.erase(str.length() - trailingWhitespace);

		return str;
	}

	std::string getFilePath(const std::string& name)
	{
		std::string::size_type p = normalizeFilePath(name).find_last_of(pathDelimiter);
		return (p == std::string::npos) ? std::string() : name.substr(0, ++p);
	}

	std::string getFileName(const std::string& fullPath)
	{
		std::string::size_type p = normalizeFilePath(fullPath).find_last_of(pathDelimiter);
		return (p  == std::string::npos) ? fullPath : fullPath.substr(p + 1);
	}

	std::string removeUpDir(std::string name)
	{
		std::string::size_type dotsPos = name.find("..");
		if (dotsPos != std::string::npos)
			name = getFilePath( name.substr(0, dotsPos - 1) ) + name.substr(dotsPos + 3);
		return name;
	}

	std::string normalizeFilePath(std::string s)
	{
		ET_ITERATE(s, char&, i, if (i == invalidPathDelimiter) i = pathDelimiter);
		return s;
	}

	std::string getFileExt(std::string name)
	{
		size_t dotPos = name.find_last_of('.');
		return (dotPos == std::string::npos) ? "" : name.substr(++dotPos);
	}

	float extractFloat(std::string& s)
	{
		size_t len = s.length();
		const char* data = s.c_str();
		bool hasMinus = data[0] == '-';
		bool dotFound = false;
		size_t offset = static_cast<size_t>(hasMinus);
		float value = 0.0f;
		float scale = 1.0f;
		for (; offset < len; ++offset)
		{
			char c = data[offset];
			if (((c >= '0') && (c <= '9')) || (c == '.'))
			{
				if (c == '.')
				{
					if (dotFound) break;
					dotFound = true;
					scale = 0.1f;
				}
				else 
				{
					float cValue = static_cast<float>(c - '0');
					if (dotFound)
					{
						value += cValue * scale;
						scale /= 10.0f;
					}
					else
					{
						value = value * 10.0f + cValue;
						scale *= 10.0f;
					}
				}
			}
			else 
			{
				break;
			}
		}

		s.erase(0, offset);
		return hasMinus ? -value : value;
	}

	StringList split(const std::string& s, const std::string& delim)
	{
		StringList result;
		
		size_t startIndex = 0;
		size_t separatorIndex = s.find_first_of(delim, startIndex);

		while (separatorIndex != std::string::npos)
		{
			result.push_back(s.substr(startIndex, separatorIndex - startIndex));
			startIndex = separatorIndex + 1;
			separatorIndex = s.find_first_of(delim, startIndex);
		}

		if (startIndex < s.size())
			result.push_back(s.substr(startIndex));

		return result;
	}

	template <typename R, int C>
	R strToVector(const std::string& s, const std::string& delimiter)
	{
		R result;
		size_t index = 0;
		StringList values = split(s, delimiter);
		for (auto& i : values)
		{
			result[index++] = strToFloat(i);
			if (index >= C) break;
		}
		return result;
	}

	vec2 strToVector2(const std::string& s, const std::string& delimiter)
		{ return strToVector<vec2, 2>(s, delimiter); }

	vec3 strToVector3(const std::string& s, const std::string& delimiter)
		{ return strToVector<vec3, 3>(s, delimiter); }

	vec4 strToVector4(const std::string& s, const std::string& delimiter)
		{ return strToVector<vec4, 4>(s, delimiter); }

	int hexCharacterToInt(int c)
	{
		if ((c >= '0') && (c <= '9')) 
			return c - '0';
		else if ((c >= 'a') && (c <= 'f')) 
			return c - 'a' + 10;
		else 
			return 0;
	}

	vec4 strHexToVec4(const std::string& s)
	{
		vec4 result;
		uint32_t value = 0;
		int l = etMin(8, static_cast<int>(s.size()));

		uint32_t scale = 1;
		const char* cstr = s.c_str();
		for (int i = l - 1; i >= 0; --i)
		{
			value += scale * static_cast<uint32_t>(hexCharacterToInt(tolower(cstr[i])));
			scale *= 16;
		}

		unsigned char a = static_cast<unsigned char>((value & 0xff000000) >> 24);
		unsigned char b = static_cast<unsigned char>((value & 0x00ff0000) >> 16);
		unsigned char g = static_cast<unsigned char>((value & 0x0000ff00) >> 8);
		unsigned char r = static_cast<unsigned char>((value & 0x000000ff) >> 0);

		result.x = static_cast<float>(r) / 255.0f;
		result.y = static_cast<float>(g) / 255.0f;
		result.z = static_cast<float>(b) / 255.0f;
		result.w = static_cast<float>(a) / 255.0f;

		return result;
	}

	vec4 strHexToVec4(const std::wstring& s)
	{
		vec4 result;
		uint32_t value = 0;
		int l = etMin(8, static_cast<int>(s.size()));

		uint32_t scale = 1;
		const wchar_t* cstr = s.c_str();
		for (int i = l - 1; i >= 0; --i)
		{
			value += scale * static_cast<uint32_t>(hexCharacterToInt(::tolower(cstr[i])));
			scale *= 16;
		}
		
		unsigned char a = static_cast<unsigned char>((value & 0xff000000) >> 24);
		unsigned char b = static_cast<unsigned char>((value & 0x00ff0000) >> 16);
		unsigned char g = static_cast<unsigned char>((value & 0x0000ff00) >> 8);
		unsigned char r = static_cast<unsigned char>((value & 0x000000ff) >> 0);

		result.x = static_cast<float>(r) / 255.0f;
		result.y = static_cast<float>(g) / 255.0f;
		result.z = static_cast<float>(b) / 255.0f;
		result.w = static_cast<float>(a) / 255.0f;

		return result;
	}

	std::ostream& operator << (std::ostream& stream, const StringList& list)
	{
		stream << "{" << std::endl;
		ET_ITERATE(list, auto, i, stream << "\t" << i << std::endl);
		stream << "}" << std::endl;
		
		return stream;
	}

}

std::string et::removeWhitespace(const std::string& s)
{
	StringDataStorage result(s.size() + 1, 0);
	ET_ITERATE(s, auto, c, if (!isWhitespaceChar(c)) result.push_back(c));
	return std::string(result.data());
}