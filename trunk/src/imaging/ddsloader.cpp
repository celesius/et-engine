/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/stream.h>
#include <et/imaging/ddsloader.h>
#include <et/opengl/opengl.h>

using namespace et;

struct DDS_PIXELFORMAT
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;
	uint32_t dwRGBBitCount;
	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwABitMask;
};

struct DDS_HEADER
{
	uint32_t dwSize;
	uint32_t dwHeaderFlags;
	uint32_t dwHeight;
	uint32_t dwWidth;
	uint32_t dwPitchOrLinearSize;
	uint32_t dwDepth;
	uint32_t dwMipMapCount;
	uint32_t dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t dwSurfaceFlags;
	uint32_t dwCubemapFlags;
	uint32_t dwReserved2[3];
};

#define DDS_ALPHA_PIXELS                    0x01
#define DDS_ALPHA                           0x02
#define DDS_FOURCC                          0x04
#define DDS_RGB                             0x40
#define DDS_RGBA                            0x41
#define DDSD_CAPS                           0x1
#define DDSD_HEIGHT                         0x2
#define DDSD_WIDTH                          0x4
#define DDSD_PITCH                          0x8
#define DDSD_PIXELFORMAT                    0x1000
#define DDSD_MIPMAPCOUNT                    0x20000
#define DDSD_LINEARSIZE                     0x80000
#define DDSD_DEPTH                          0x800000
#define DDSCAPS_COMPLEX                     0x8
#define DDSCAPS_MIPMAP                      0x400000
#define DDSCAPS_TEXTURE                     0x1000

const uint32_t DDS_HEADER_ID = ET_CHARACTER_LITERAL(' ', 'S', 'D', 'D');

const uint32_t FOURCC_DXT1 = ET_CHARACTER_LITERAL('1', 'T', 'X', 'D');
const uint32_t FOURCC_DXT3 = ET_CHARACTER_LITERAL('3', 'T', 'X', 'D');
const uint32_t FOURCC_DXT5 = ET_CHARACTER_LITERAL('5', 'T', 'X', 'D');
const uint32_t FOURCC_ATI1 = ET_CHARACTER_LITERAL('1', 'I', 'T', 'A');
const uint32_t FOURCC_ATI2 = ET_CHARACTER_LITERAL('1', 'I', 'T', 'A');

void DDSLoader::loadInfoFromStream(std::istream& source, TextureDescription& desc)
{
	uint32_t headerId = 0;
	source.read((char*)&headerId, sizeof(headerId));

	if (headerId != DDS_HEADER_ID) return;

	DDS_HEADER header = { };
	source.read((char*)&header, sizeof(header));

	desc.size = vec2i(static_cast<int>(header.dwWidth), static_cast<int>(header.dwHeight));
	desc.mipMapCount = etMax(static_cast<uint32_t>(1), header.dwMipMapCount);
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
            desc.internalformat = static_cast<int32_t>(desc.format);
            desc.type = GL_UNSIGNED_BYTE;
            break;
        }
            
#if defined(GL_RG) && defined(GL_RG16)
	case 34:
		{
			desc.channels = 2;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_RG16;
			desc.format = GL_RG;
			desc.type = GL_UNSIGNED_SHORT;
			break;   
		}
#endif
			
#if defined(GL_RGBA16)
	case 36:
		{
			desc.bitsPerPixel = 16 * desc.channels;
			desc.channels = 4;
			desc.internalformat = GL_RGBA16;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_SHORT;
			break;   
		}
#endif
			
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
            
#if defined(GL_RG32F)
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
			
#if defined(GL_RGBA16F)
	case 113:
		{
			desc.channels = 4;
			desc.bitsPerPixel = 16 * desc.channels;
			desc.internalformat = GL_RGBA16F;
			desc.format = GL_RGBA;
			desc.type = GL_HALF_FLOAT;
			break;   
		}
#endif
			
#if defined(GL_RGBA32F)
	case 116:
		{
			desc.channels = 4;
			desc.bitsPerPixel = 32 * desc.channels;
			desc.internalformat = GL_RGBA32F;
			desc.format = GL_RGBA;
			desc.type = GL_FLOAT;
			break;   
		}
#endif
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
			etCopyMemory(fourcc_str, &header.ddspf.dwFourCC, 4);
			log::error("Unsupported FOURCC: %u, text: %s", header.ddspf.dwFourCC, fourcc_str);
			return;
		}
	};
}

void DDSLoader::loadFromStream(std::istream& source, TextureDescription& desc)
{
	if (source.fail()) 
	{
		log::error("Unable to load DDS image from stream: %s", desc.origin().c_str());
		return;
	}

	loadInfoFromStream(source, desc);
	if (desc.dataSizeForAllMipLevels())
	{
		desc.data = BinaryDataStorage(desc.dataSizeForAllMipLevels());
		source.read(desc.data.binary(), static_cast<std::streamsize>(desc.data.dataSize()));
	}
}

void DDSLoader::loadFromFile(const std::string& path, TextureDescription& desc)
{
	InputStream file(path, StreamMode_Binary);
	if (file.valid())
	{
		desc.setOrigin(path);
		loadFromStream(file.stream(), desc);
	}
}

void DDSLoader::loadInfoFromFile(const std::string& path, TextureDescription& desc)
{
	InputStream file(path, StreamMode_Binary);
	if (file.valid())
	{
		desc.setOrigin(path);
		loadInfoFromStream(file.stream(), desc);
	}
}
