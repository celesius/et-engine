#include <fstream>
#include <et/core/tools.h>
#include <et/opengl/opengl.h>
#include <et/loaders/pvrloader.h>

using namespace et;

enum PVRFormat
{
	PVRFormat_PVRTC2 = 0x18,
	PVRFormat_PVRTC4 = 0x19
};

struct PVRHeader
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

const int PVRFormatMask = 0xff;


void PVRLoader::loadInfoFromStream(std::istream& stream, TextureDescription& desc)
{
	PVRHeader header = { };
	stream.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (header.dwPVR != '!RVP') return;

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
	if (format == PVRFormat_PVRTC2)
	{
		desc.compressed = true;
		desc.channels = 3 + header.dwAlphaBitMask;
		desc.internalformat = header.dwAlphaBitMask ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	}
	else if (format == PVRFormat_PVRTC4)
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
