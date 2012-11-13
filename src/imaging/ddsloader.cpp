/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>

#include <et/opengl/opengl.h>
#include <et/imaging/ddsloader.h>

using namespace et;

void DDSLoader::loadInfoFromStream(std::istream& source, TextureDescription& desc)
{
	long headerId = 0;
	source.read((char*)&headerId, sizeof(headerId));

	if (headerId != DDS_HEADER_ID) return;

	DDS_HEADER header = { };
	source.read((char*)&header, sizeof(header));

	desc.size = vec2i(header.dwWidth, header.dwHeight);
	desc.mipMapCount = etMax(1l, header.dwMipMapCount);
	desc.layersCount = 1;

	if (desc.mipMapCount > 1)
	{
		int lastLevelScale = intPower(2, desc.mipMapCount);
		while ((desc.size.x / lastLevelScale < 4) && (desc.size.y / lastLevelScale < 4))
		{
			desc.mipMapCount--;
			lastLevelScale = intPower(2, desc.mipMapCount);
		}
	}

	bool hasAlpha = header.ddspf.dwFlags & DDS_ALPHA_PIXELS;
    (void)hasAlpha;
    
	switch (header.ddspf.dwFourCC)
	{
        case 0:
        {
            desc.channels = header.ddspf.dwRGBBitCount / 8;
            desc.bitsPerPixel = header.ddspf.dwRGBBitCount;
            desc.format = desc.channels == 3 ? GL_RGB : GL_RGBA;
            desc.internalformat = desc.format;
            desc.type = GL_UNSIGNED_BYTE;
            break;
        }
            
	case 34:
		{
			desc.channels = 2;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_RG16;
			desc.format = GL_RG;
			desc.type = GL_UNSIGNED_SHORT;
			break;   
		}

	case 36:
		{
			desc.bitsPerPixel = 16 * desc.channels;
			desc.channels = 4;
			desc.internalformat = GL_RGBA16;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_SHORT;
			break;   
		}
            
#if defined(GL_R16F)
	case 111: 
		{
			desc.channels = 1;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_R16F;
			desc.format = GL_RED;
			desc.type = GL_HALF_FLOAT;
			break;   
		}
#endif
            
#if defined(GL_RG16F) 
	case 112:
		{
			desc.channels = 2;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_RG16F;
			desc.format = GL_RG;
			desc.type = GL_HALF_FLOAT;
			break;   
		}
#endif
            
#if defined(GL_R32F)
	case 114:
		{
			desc.channels = 1;
			desc.bitsPerPixel = 32 * desc.channels;
			desc.internalformat = GL_R32F;
			desc.format = GL_RED;
			desc.type = GL_FLOAT;
			break;   
		}
#endif
            
#if defined (GL_RG32F)
	case 115:
		{
			desc.channels = 2;
			desc.bitsPerPixel = 32 * desc.channels;
			desc.internalformat = GL_RG32F;
			desc.format = GL_RG;
			desc.type = GL_FLOAT;
			break;   
		}		 
#endif
	case 113:
		{
			desc.channels = 4;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_RGBA16F;
			desc.format = GL_RGBA;
			desc.type = GL_HALF_FLOAT;
			break;   
		}

	case 116:
		{
			desc.channels = 4;
			desc.bitsPerPixel = 32 * desc.channels;
			desc.internalformat = GL_RGBA32F;
			desc.format = GL_RGBA;
			desc.type = GL_FLOAT;
			break;   
		}

#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT) 
	case FOURCC_DXT1:
		{
			desc.compressed = true;
			desc.channels = 4;
			desc.bitsPerPixel = 1 * desc.channels;
			desc.internalformat = hasAlpha ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			desc.format = hasAlpha ? GL_RGB : GL_RGBA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		}
#endif
            
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
	case FOURCC_DXT3:
		{
			desc.compressed = true;
			desc.channels = 4;
			desc.bitsPerPixel = 2 * desc.channels;
			desc.internalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		}
#endif
            
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) 
	case FOURCC_DXT5:
		{
			desc.compressed = true;
			desc.channels = 4;
			desc.bitsPerPixel = 2 * desc.channels;
			desc.internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		}
#endif
            
#if defined(GL_COMPRESSED_RG_RGTC2)
	case FOURCC_ATI2:
		{
			desc.compressed = true;
			desc.channels = 2;
			desc.bitsPerPixel = 4 * desc.channels;
			desc.internalformat = GL_COMPRESSED_RG_RGTC2;
			desc.format = GL_RGB;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		}
#endif
            
	default: 
		{
			char fourcc_str[5] = { };
			memcpy(fourcc_str, &header.ddspf.dwFourCC, 4);
			std::cout << "Unsupported FOURCC: " << header.ddspf.dwFourCC << ", text: " << fourcc_str << std::endl;
			return;
		}
	};
}

void DDSLoader::loadFromStream(std::istream& source, TextureDescription& desc)
{
	if (source.fail()) 
	{
		std::cout << "Unable to load DDS image from stream: " << desc.source << std::endl;
		return;
	}

	loadInfoFromStream(source, desc);
	if (desc.dataSizeForAllMipLevels())
	{
		desc.data = BinaryDataStorage(desc.dataSizeForAllMipLevels());
		source.read(desc.data.binary(), desc.data.dataSize());
	}
}

void DDSLoader::loadFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios_base::binary);
	loadFromStream(file, desc);
}

void DDSLoader::loadInfoFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios_base::binary);
	loadInfoFromStream(file, desc);
}
