/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <et/app/application.h>
#include <et/core/containers.h>
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
#if ET_PLATFORM_WIN
		extensions.push_back(".dds");
		extensions.push_back(".png");
		extensions.push_back(".jpg");
		extensions.push_back(".jpeg");
#elif ET_PLATFORM_IOS
		extensions.push_back(".pvr");
		extensions.push_back(".png");
		extensions.push_back(".jpg");
		extensions.push_back(".jpeg");
#elif ET_PLATFORM_MAC
		extensions.push_back(".dds");
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

std::string TextureLoader::resolveScalableFileName(const std::string& fileName, size_t screenScale, std::string* extPtr, bool silent)
{
	std::string result = fileName;
	size_t lastDotPos = fileName.find_last_of('.');

	bool found = false;

	if ((lastDotPos == std::string::npos) || (lastDotPos < fileName.size() - 5))
	{
		const StringList& ext = TextureLoader::preferredExtensions();
		for (StringList::const_iterator i = ext.begin(), e = ext.end(); i != e; ++i)
		{
			std::string name = application().environment().resolveScalableFileName(fileName + (*i), screenScale);
			if (fileExists(name))
			{
				result = name;
				found = true;
				break;
			}
		}
        
        if (!found)
        {
            std::string extractedFileName = getFileName(fileName);
            if (extractedFileName != fileName)
            {
                for (StringList::const_iterator i = ext.begin(), e = ext.end(); i != e; ++i)
                {
                    std::string name = application().environment().resolveScalableFileName(extractedFileName + (*i), screenScale);
                    if (fileExists(name))
                    {  
                        result = name;
                        found = true;
                        break;
                    }
                }
            }
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
			std::cout << "Unable to resolve file name for: " << fileName << std::endl;
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

TextureDescription::Pointer TextureLoader::loadDescription(const std::string& name, size_t scaleFactor, bool initWithZero)
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
		std::cout << "Unable to load texture description from file: " << name << std::endl;
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
	else
	{
//		std::cout << "Unable to load texture from file: " << name << std::endl;
	}

	return TextureDescription::Pointer(desc);
}