/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <et/core/tools.h>
#include <et/core/serialization.h>
#include <et/gui/font.h>
#include <et/app/application.h>

using namespace et;
using namespace et::gui;

static const CharDescriptor emptyCharDescriptor = { };

FontData::FontData() : _size(0)
{
}

FontData::FontData(RenderContext* rc, const std::string& fileName, TextureCache& cache) :  _size(0)
{
	loadFromFile(rc, fileName, cache);
}

FontData::~FontData()
{

}

void FontData::loadFromFile(RenderContext* rc, const std::string& fileName, TextureCache& cache)
{
	_chars.clear();
	_boldChars.clear();
	_texture = Texture();
	std::string resolvedFileName = application().environment().resolveScalableFileName(fileName, rc->screenScaleFactor());

	if (!fileExists(resolvedFileName)) 
	{
		std::cout << "Unable to find font file: "  << fileName << std::endl;
		return;
	}

	std::ifstream fontFile(resolvedFileName.c_str(), std::ios::binary);
	if (fontFile.fail())
	{
		std::cout << "Unable to open font file: " << resolvedFileName << std::endl;
		return;
	}

	std::string fontFileDir = getFilePath(resolvedFileName);

	int version = deserializeInt(fontFile);
	if (version == FONT_VERSION_1)
	{
		_face = deserializeString(fontFile);
		_size = deserializeInt(fontFile);
		std::string textureFile = deserializeString(fontFile);
		std::string layoutFile = deserializeString(fontFile);

		std::string textureFileName = fontFileDir + textureFile;
		_texture = rc->textureFactory().loadTexture(fileExists(textureFileName) ? textureFileName : textureFile, cache);

		int charCount = deserializeInt(fontFile);
		for (int i = 0; i < charCount; ++i)
		{
			CharDescriptor desc = { };
			fontFile.read(reinterpret_cast<char*>(&desc), sizeof(desc));
			if ((desc.params & CharParameter_Bold) == CharParameter_Bold)
				_boldChars[desc.value] = desc;
			else
				_chars[desc.value] = desc;
		}
	}
}

const CharDescriptor& FontData::charDescription(short c) const
{
	CharDescriptorMap::const_iterator i = _chars.find(c);
	return (i != _chars.end()) ? i->second : emptyCharDescriptor;
}

float FontData::lineHeight() const
{
	if (_chars.size())
	{
		CharDescriptorMap::const_iterator i = _chars.begin();
		return i->second.size.y;
	}
	else
	{
		return static_cast<float>(_size);
	}
}

const CharDescriptor& FontData::boldCharDescription(short c) const
{
	CharDescriptorMap::const_iterator i = _boldChars.find(c);
	return (i != _boldChars.end()) ? i->second : emptyCharDescriptor;
}

vec2 FontData::measureStringSize(const CharDescriptorList& s) const
{
	vec2 sz(0.0f, 0.0f);
	for (CharDescriptorList::const_iterator i = s.begin(), e = s.end(); i != e; ++i)
	{
		const CharDescriptor& desc = *i;
		sz.x += desc.size.x;
		sz.y = etMax(desc.size.y, sz.y);
	}
	return sz;
}

vec2 FontData::measureStringSize(const std::string& s, bool formatted) const
{
	return measureStringSize(formatted ? parseString(s) : buildString(s));
}

CharDescriptorList FontData::buildString(const std::string& s, bool formatted) const
{
	if (formatted)
		return parseString(s);

	CharDescriptorList result;

	for (std::string::const_iterator i = s.begin(), e = s.end(); i != e; ++i)
		result.push_back(charDescription(*i));

	return result;
}

CharDescriptorList FontData::parseString(const std::string& s) const
{
	CharDescriptorList result;
	
	const char tagOpening = '<';
	const char tagClosing = '>';
	const char tagClosingId = '/';
	const std::string tagBold("b");
	const std::string tagColor("color");
	const std::string tagColorOpen("color=#");

	std::deque<vec4> colors;
	colors.push_front(vec4(1.0f));

	int nBoldTags = 0;
	int nColorTags = 0;
	bool readingTag = false;
	bool closingTag = false;
	std::string tag;

	for (std::string::const_iterator i = s.begin(), e = s.end(); i != e; ++i)
	{
		const char c = *i;
		if (c == tagOpening)
		{
			readingTag = true;
			closingTag = false;
			tag = std::string();
		}
		else if (c == tagClosing)
		{
			if (readingTag)
			{
				if (closingTag)
				{
					if (tag == tagBold)
					{
						if (nBoldTags)
							--nBoldTags;
						else
							std::cout << "WARNING: closing <b> tag without opening in string: " << s << std::endl;
					}
					else if (tag.find_first_of(tagColor) == 0)
					{
						if (nColorTags)
						{
							--nColorTags;
							colors.pop_front();
						}
						else
						{
							std::cout << "WARNING: closing <color> tag without opening in string: " << s << std::endl;
						}
					}
					else 
					{
						std::cout << "Unknown tag `" << tag << "` passed in string: " << s << std::endl;
					}
				}
				else
				{
					if (tag == tagBold)
					{
						nBoldTags++;
					}
					else if (tag.find(tagColorOpen) == 0)
					{
						nColorTags++;
						tag.erase(0, tagColorOpen.size());
						colors.push_front(strHexToVec4(tag));
					}
					else
					{
						std::cout << "Unknown tag `" << tag << "` passed in string: " << s << std::endl;
					}
				}
				readingTag = false;
				closingTag = false;
			}
		}
		else 
		{
			if (readingTag)
			{
				if (c == tagClosingId)
				{
					closingTag = true;
				}
				else if (!isWhitespaceChar(c))
				{
					tag += c;
				}
			}
			else
			{
				result.push_back(nBoldTags ? boldCharDescription(c) : charDescription(c));
				result.back().color = colors.front();
			}
		}
	}

	return result;
}