#include <fstream>
#include <et/core/tools.h>
#include <et/opengl/opengl.h>
#include <et/imaging/pvrloader.h>

using namespace et;

enum PVRFormat
{
	PVRVersion3Format_PVRTC_2bpp_RGB = 0,
	PVRVersion3Format_PVRTC_2bpp_RGBA = 1,
	PVRVersion3Format_PVRTC_4bpp_RGB = 2,
	PVRVersion3Format_PVRTC_4bpp_RGBA = 3,
    
	PVRVersion2Format_PVRTC2 = 0x18,
	PVRVersion2Format_PVRTC4 = 0x19
};

enum PVRChannelType
{
    PVRChannelType_UnsignedByteNorm = 0,
    PVRChannelType_UnsignedByte = 2,
    
    PVRChannelType_UnsignedShortNorm = 4,
    PVRChannelType_UnsignedShort = 6,
    
    PVRChannelType_UnsignedIntNorm = 8,
    PVRChannelType_UnsignedInt = 10,
};

namespace et
{
    struct PVRHeader2
    {
        unsigned int dwHeaderSize;		/* size of the structure */
        unsigned int dwHeight;			/* height of surface to be created */
        unsigned int dwWidth;			/* width of input surface */
        unsigned int dwMipMapCount;		/* number of MIP-map levels requested */
        unsigned int dwpfFlags;			/* pixel format flags */
        unsigned int dwDataSize;		/* Size of the compress data */
        unsigned int dwBitCount;		/* number of bits per pixel */
        unsigned int dwRBitMask;		/* mask for red bit */
        unsigned int dwGBitMask;		/* mask for green bits */
        unsigned int dwBBitMask;		/* mask for blue bits */
        unsigned int dwAlphaBitMask;	/* mask for alpha channel */
        unsigned int dwPVR;				/* should be 'P' 'V' 'R' '!' */
        unsigned int dwNumSurfs;		/* number of slices for volume textures or skyboxes */
    };
    
    struct PVRHeader3
    {
        unsigned int version;			
        unsigned int flags;		  
        uint64_t pixelFormat;		
        unsigned int colourSpace;		
        unsigned int channelType;		
        unsigned int height;		 
        unsigned int width;		  
        unsigned int depth;		  
        unsigned int numSurfaces;		
        unsigned int numFaces;	   
        unsigned int numMipmaps;	 
        unsigned int metaDataSize;   
    };	
}

const unsigned int PVRFormatMask = 0xff;
const unsigned int PVRHeaderV3Version = 0x03525650;

void PVRLoader::loadInfoFromV2Header(const PVRHeader2& header, TextureDescription& desc)
{
	desc.size = vec2i(header.dwWidth, header.dwHeight);
	desc.type = GL_UNSIGNED_BYTE;
	desc.bitsPerPixel = header.dwBitCount;
	desc.mipMapCount = header.dwMipMapCount + 1;
	desc.format = header.dwAlphaBitMask ? GL_RGBA : GL_RGB;
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
    
	size_t format = header.dwpfFlags & PVRFormatMask;
	if (format == PVRVersion2Format_PVRTC2)
	{
		desc.compressed = true;
		desc.channels = 3 + header.dwAlphaBitMask;
		desc.internalformat = header.dwAlphaBitMask ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	}
	else if (format == PVRVersion2Format_PVRTC4)
	{
		desc.compressed = true;
		desc.channels = 3 + header.dwAlphaBitMask;
		desc.internalformat = header.dwAlphaBitMask ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	}
	else
	{
		std::cout << "Unresolved PVR format: " << format << std::endl;
	}    
}

void PVRLoader::loadInfoFromV3Header(const PVRHeader3& header, TextureDescription& desc)
{
	desc.size = vec2i(header.width, header.height);
	desc.mipMapCount = header.numMipmaps + 1;
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
    
    if ((header.channelType == PVRChannelType_UnsignedByte) || (header.channelType == PVRChannelType_UnsignedByteNorm))
        desc.type = GL_UNSIGNED_BYTE;
    else if ((header.channelType == PVRChannelType_UnsignedShort) || (header.channelType == PVRChannelType_UnsignedShortNorm))
        desc.type = GL_UNSIGNED_SHORT;
    else if ((header.channelType == PVRChannelType_UnsignedInt) || (header.channelType == PVRChannelType_UnsignedIntNorm))
        desc.type = GL_UNSIGNED_INT;
    else 
        std::cout << "Unsupported PVR channel type: " << header.channelType << std::endl;
    
    if (header.pixelFormat == PVRVersion3Format_PVRTC_2bpp_RGB)
	{
		desc.compressed = true;
		desc.channels = 3;
        desc.bitsPerPixel = 2;
        desc.format = GL_RGB;
		desc.internalformat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	}
    else if (header.pixelFormat == PVRVersion3Format_PVRTC_2bpp_RGBA)
	{
		desc.compressed = true;
		desc.channels = 4;
        desc.bitsPerPixel = 2;
        desc.format = GL_RGBA;
		desc.internalformat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	}
    else if (header.pixelFormat == PVRVersion3Format_PVRTC_4bpp_RGB)
	{
		desc.compressed = true;
		desc.channels = 3;
        desc.bitsPerPixel = 4;
        desc.format = GL_RGB;
		desc.internalformat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	}
    else if (header.pixelFormat == PVRVersion3Format_PVRTC_4bpp_RGBA)
	{
		desc.compressed = true;
        desc.bitsPerPixel = 4;
		desc.channels = 4;
        desc.format = GL_RGBA;
		desc.internalformat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
	}
	else
	{
		std::cout << "Unresolved PVR pixel format: " << header.pixelFormat << std::endl;
	}   
}

void PVRLoader::loadInfoFromStream(std::istream& stream, TextureDescription& desc)
{
    std::istream::off_type offset = stream.tellg();
    
	PVRHeader2 header2 = { };
	stream.read(reinterpret_cast<char*>(&header2), sizeof(header2));
	if (header2.dwPVR == '!RVP') 
    {
        PVRLoader::loadInfoFromV2Header(header2, desc);
    }
    else 
    {
        stream.seekg(offset, std::ios_base::beg);
        PVRHeader3 header3 = { };
        stream.read(reinterpret_cast<char*>(&header3), sizeof(header3));
        
        if (header3.version == PVRHeaderV3Version)
        {
            PVRLoader::loadInfoFromV3Header(header3, desc);
            stream.seekg(header3.metaDataSize, std::ios_base::cur);
        }
        else 
            std::cout << "Unrecognized PVR input stream" << std::endl;
    }
}

void PVRLoader::loadInfoFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios_base::binary);
	loadInfoFromStream(file, desc);
}

void PVRLoader::loadFromStream(std::istream& stream, TextureDescription& desc)
{
	loadInfoFromStream(stream, desc);

	desc.data = BinaryDataStorage(desc.dataSizeForAllMipLevels());
	stream.read(desc.data.binary(), desc.data.dataSize());
}

void PVRLoader::loadFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios_base::binary);
	loadFromStream(file, desc);
}
