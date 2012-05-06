/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <string>

#include <et/app/application.h>
#include <et/core/containers.h>
#include <et/resources/textureloader.h>
#include <et/imaging/pngloader.h>
#include <et/imaging/ddsloader.h>
#include <et/imaging/pvrloader.h>

#define ET_TEXTURE_DEBUG_LOAD	0

using namespace et;

static StringList extensions;

const StringList& TextureLoader::preferredExtensions()
{
	if (extensions.size() == 0)
	{
#if ET_PLATFORM_WIN
		extensions.push_back(".dds");
		extensions.push_back(".png");
#elif ET_PLATFORM_IOS
		extensions.push_back(".pvr");
		extensions.push_back(".png");
#elif ET_PLATFORM_MAC
		extensions.push_back(".dds");
		extensions.push_back(".png");
#else
	#error Please, define preferred extensions list for current platform.
#endif
	}

	return extensions;
}

std::string TextureLoader::resolveScalableFileName(const std::string& fileName, size_t screenScale, std::string* extPtr, bool silent)
{
	if (extPtr)
		*extPtr = std::string();

	std::string result = fileName;
	size_t lastDotPos = fileName.find_last_of('.');

	bool found = false;

	if ((lastDotPos == std::string::npos) || (lastDotPos < fileName.length() - 4))
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

const TextureDescription TextureLoader::loadDescription(const std::string& name, size_t scaleFactor, bool initWithZero)
{
	TextureDescription desc;
	desc.target = GL_TEXTURE_2D;
	std::string ext;
	
#if (ET_TEXTURE_DEBUG_LOAD)
	std::string fileName = resolveFileName("debug/debug", &ext);
#else
	std::string fileName = resolveScalableFileName(name, scaleFactor, &ext);
#endif
	
	if (ext == ".png")
	{
		PNGLoader::loadInfoFromFile(fileName, desc);
	}
	else if (ext == ".dds")
	{
		DDSLoader::loadInfoFromFile(fileName, desc);
	}
	else if (ext == ".pvr")
	{
		PVRLoader::loadInfoFromFile(fileName, desc);
	}
	else
	{
		std::cout << "Unable to load texture description from file: " << name << std::endl;
	}

	if (initWithZero)
	{
		desc.data = BinaryDataStorage(desc.dataSizeForAllMipLevels());
		desc.data.fill(0);
	}

	return desc;
}

const TextureDescription TextureLoader::load(const std::string& name, size_t scaleFactor)
{
	TextureDescription desc;
	desc.target = GL_TEXTURE_2D;
	std::string ext;
	
#if (ET_TEXTURE_DEBUG_LOAD)
	std::string fileName = resolveFileName("debug/debug", &ext);
#else
	std::string fileName = resolveScalableFileName(name, scaleFactor, &ext, true);
#endif
	
	if (ext == ".png")
	{
		PNGLoader::loadFromFile(fileName, desc);
	}
	else if (ext == ".dds")
	{
		DDSLoader::loadFromFile(fileName, desc);
	}
	else if (ext == ".pvr")
	{
		PVRLoader::loadFromFile(fileName, desc);
	}
	else 
	{
		std::cout << "Unable to load texture from file: " << name << std::endl;
	}

	return desc; 
}