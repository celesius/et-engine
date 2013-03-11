/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/app/application.h>
#include <et/core/tools.h>
#include <et/resources/textureloader.h>
#include <et/imaging/pngloader.h>
#include <et/imaging/ddsloader.h>
#include <et/imaging/pvrloader.h>
#include <et/imaging/jpgloader.h>

using namespace et;

static StringList extensions;

const StringList& TextureLoader::preferredExtensions()
{
	if (extensions.size() == 0)
	{
#if (ET_PLATFORM_WIN || ET_PLATFORM_MAC)
		extensions.push_back(".dds");
		extensions.push_back(".png");
		extensions.push_back(".jpg");
		extensions.push_back(".jpeg");
#elif (ET_PLATFORM_IOS || ET_PLATFORM_ANDROID)
		extensions.push_back(".pvr");
		extensions.push_back(".png");
		extensions.push_back(".jpg");
		extensions.push_back(".jpeg");
#else
#
#	error Please, define preferred extensions list for current platform.
#
#endif
	}

	return extensions;
}

std::string TextureLoader::resolveScalableFileName(const std::string& fileName,
	size_t screenScale, std::string* extPtr, bool silent)
{
	std::string result = fileName;
	size_t lastDotPos = fileName.find_last_of('.');
	std::string extractedFileName = getFileName(fileName);

	bool found = false;

	if ((lastDotPos == std::string::npos) || (lastDotPos < fileName.size() - 5))
	{
		const StringList& ext = TextureLoader::preferredExtensions();
		ET_ITERATE(ext, const auto&, i,
		{
			std::string name = fileName + i;
			name = application().environment().resolveScalableFileName(name, screenScale);
			if (fileExists(name))
			{
				result = name;
				found = true;
				break;
			}
		})
        
        if (!found && (extractedFileName != fileName))
        {
			ET_ITERATE(ext, const auto&, i,
			{
				std::string name = extractedFileName + i;
				name = application().environment().resolveScalableFileName(name, screenScale);
				if (fileExists(name))
				{
					result = name;
					found = true;
					break;
				}
			})
		}
	}
	else
	{
		std::string name = application().environment().resolveScalableFileName(fileName, screenScale);
		found = fileExists(name);
		if (!found)
        {
            name = application().environment().resolveScalableFileName(getFileName(fileName), screenScale);
            found = fileExists(name);
        }
		
        if (!found)
        {
            name = resolveScalableFileName(removeFileExt(fileName), screenScale, extPtr, silent);
            found = fileExists(name);
        }
        
        if (found)
            result = name;
    }	
    
	if (!found)
	{
		if (!silent)
			log::warning("Unable to resolve file name for: %s", fileName.c_str());
	}
	else if (extPtr)
	{
		*extPtr = result.substr(result.rfind('.'));
		lowercase(*extPtr);
	}

	return result;
}

std::string TextureLoader::resolveFileName(const std::string& name, std::string* ext, bool silent)
{
	return TextureLoader::resolveScalableFileName(name, 1, ext, silent);
}

TextureDescription::Pointer TextureLoader::loadDescription(const std::string& name,
	size_t scaleFactor, bool initWithZero)
{
	std::string ext;
	std::string fileName = resolveScalableFileName(name, scaleFactor, &ext);
	
	TextureDescription* desc = nullptr;
	if (ext == ".png")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		desc->source = fileName;
		PNGLoader::loadInfoFromFile(fileName, *desc);
	}
	else if (ext == ".dds")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		desc->source = fileName;
		DDSLoader::loadInfoFromFile(fileName, *desc);
	}
	else if (ext == ".pvr")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		desc->source = fileName;
		PVRLoader::loadInfoFromFile(fileName, *desc);
	}
	else if ((ext == ".jpg") || (ext == ".jpeg"))
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		desc->source = fileName;
		JPGLoader::loadInfoFromFile(fileName, *desc);
	}
	else
	{
		log::warning("Unable to load texture description from file: %s", name.c_str());
	}
	
	if ((desc != nullptr) && initWithZero)
	{
		desc->data = BinaryDataStorage(desc->dataSizeForAllMipLevels());
		desc->data.fill(0);
	}

	return TextureDescription::Pointer(desc);
}

TextureDescription::Pointer TextureLoader::load(const std::string& name, size_t scaleFactor)
{
	std::string ext;
	std::string fileName = resolveScalableFileName(name, scaleFactor, &ext, true);
	
	TextureDescription* desc = nullptr;
	
	if (ext == ".png")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		desc->source = fileName;
		PNGLoader::loadFromFile(fileName, *desc);
	}
	else if (ext == ".dds")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		DDSLoader::loadFromFile(fileName, *desc);
	}
	else if (ext == ".pvr")
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		PVRLoader::loadFromFile(fileName, *desc);
	}
	else if ((ext == ".jpg") || (ext == ".jpeg"))
	{
		desc = new TextureDescription;
		desc->target = GL_TEXTURE_2D;
		JPGLoader::loadFromFile(fileName, *desc);
	}

	return TextureDescription::Pointer(desc);
}