/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <sstream>
#include <et/app/application.h>
#include <et/resources/textureloader.h>
#include <et/gui/textureatlas.h>

using namespace et;
using namespace et::gui;

Image TextureAtlas::_emptyImage = Image();

rect parseRectString(std::string& s)
{
	rect result;
	int values[4] = { };
	int vIndex = 0;

	if (*s.begin() == '"')
		s.erase(0, 1);

	if (*s.rbegin() == '"')
		s.erase(s.length() - 1);

	while (s.length())
	{
		std::string::size_type dpos = s.find_first_of(";");
		if (dpos == std::string::npos)
		{
			values[vIndex++] = strToInt(s);
			break;
		}
		else 
		{
			std::string value = s.substr(0, dpos);
			values[vIndex++] = strToInt(value);
			s.erase(0, dpos+1);
		}
	}

	result.left = static_cast<float>(values[0]);
	result.top = static_cast<float>(values[1]);
	result.width = static_cast<float>(values[2]);
	result.height = static_cast<float>(values[3]);

	return result;
}

TextureAtlas::TextureAtlas() : _loaded(false)
{
}

TextureAtlas::TextureAtlas(RenderContext* rc, const std::string& filename, TextureCache& cache) : _loaded(false)
{
	loadFromFile(rc, filename, cache);
}

void TextureAtlas::loadFromFile(RenderContext* rc, const std::string& filename, TextureCache& cache)
{
	std::string resolvedFileName = application().environment().resolveScalableFileName(filename, rc->screenScaleFactor());

	std::ifstream descFile(resolvedFileName.c_str());
	if (descFile.fail()) 
	{
		std::cout << "Unable to open " << filename << " atlas." << std::endl;
		return;
	}

	std::string filePath = getFilePath(resolvedFileName);
	int lineNumber = 1;

	while (!descFile.eof())
	{
		std::string token;
		std::string line;

		descFile >> token;
		std::getline(descFile, line);

		if (token == "texture:")
		{
			std::string textureId = trim(line);
			std::string textureName = TextureLoader::resolveFileName(textureId, 0, true);
			if (!fileExists(textureName))
				textureName = TextureLoader::resolveFileName(filePath + textureId);
			_textures[textureId] = rc->textureFactory().loadTexture(textureName, cache);
		}
		else if (token == "image:")
		{
			trim(line);
			if ((*line.begin() == '{') && (*line.rbegin() == '}'))
			{
				line = line.substr(1, line.length() - 2);
				trim(line);
				std::istringstream parser(line);

				std::string imageName;
				std::string textureName;
				rect sourceRect;
				rect contentOffset;

				while (!parser.eof())
				{
					std::string aToken;
					parser >> aToken;
					if (aToken == "name:")
					{
						parser >> imageName;
						if (*imageName.begin() == '"')
							imageName.erase(0, 1);
						if (*imageName.rbegin() == '"')
							imageName.erase(imageName.length() - 1);
					}
					else if (aToken == "texture:")
					{
						parser >> textureName;
						if (*textureName.begin() == '"')
							textureName.erase(0, 1);
						if (*textureName.rbegin() == '"')
							textureName.erase(textureName.length() - 1);
					}
					else if (aToken == "rect:")
					{
						std::string sRect;
						parser >> sRect;
						sourceRect = parseRectString(sRect);
					}
					else if (aToken == "offset:")
					{
						std::string offset;
						parser >> offset;
						contentOffset = parseRectString(offset);
					}
					else
					{
						std::cout << "Unknown token at line " << lineNumber << ": " << aToken << std::endl;
					}
				}

				ImageDescriptor desc(sourceRect.origin(), sourceRect.size());
				desc.contentOffset = ContentOffset(contentOffset[0], contentOffset[1], contentOffset[2], contentOffset[3]);
				_images[imageName] = Image(_textures[textureName], desc);
			}
			else 
			{
				std::cout << "Unable to parse image token at line" << lineNumber << ": " << line;
			}
		}
		else 
		{
			if (token.length() && line.length())
				std::cout << "Unknown token at line " << lineNumber << ": " << token << line << std::endl;
		}

		++lineNumber;
	}

	_loaded = true;
}

const gui::Image& TextureAtlas::image(const std::string& key) const
{
	ImageMap::const_iterator i = _images.find(key);
	if (i == _images.end())
	{
		std::cout << "Image `" << key << "` not found in atlas." << std::endl;
		return _emptyImage;
	}
	else 
		return i->second;
}

ImageList TextureAtlas::imagesForTexture(Texture t) const
{
	ImageList result;
	for (ImageMap::const_iterator i = _images.begin(), e = _images.end(); i != e; ++i)
	{
		if (i->second.texture == t)
			result.push_back(i->second);
	}
	return result;
}

void TextureAtlas::unload()
{
	_images.clear();
	_textures.clear();
	_loaded = false;
}

Texture TextureAtlas::firstTexture() const
{
	return _textures.size() ? _textures.begin()->second : Texture();
}
