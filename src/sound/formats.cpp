#include <stdint.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <et/core/tools.h>
#include <et/sound/formats.h>

using namespace et;
using namespace et::audio;

union ChunkIdentifier
{
	char cID[4];
	unsigned char ucID[4];
	uint32_t nID;
};

struct AudioFileChunk
{
	ChunkIdentifier id;
	uint32_t size;
};

const uint32_t AIFFCommonChunkID = 'MMOC';
const uint32_t AIFFUncompressedDataChunkID = 'DNSS';
const uint32_t AIFFCompressedDataChunkID = 'DNSC';

const uint32_t WAVFileChunkID = 'FFIR';
const uint32_t WAVDataChunkID = 'atad';
const uint32_t WAVFormatChunkID = ' tmf';

#pragma pack(1)

struct WAVFileChunk
{
	ChunkIdentifier id;
	uint32_t chunkSize;
	char format[4];
};

struct WAVFormatChunk
{
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct AIFFFileChunk
{
	ChunkIdentifier ckID;
	uint32_t ckDataSize;
	ChunkIdentifier formType;
};

struct AIFFCommonChunk 
{
	uint16_t numChannels;
	uint32_t numSampleFrames;
	uint16_t sampleSize;

	union
	{
		unsigned char c_sampleRate[10];
		long double sampleRate;
	};

	ChunkIdentifier compressionType;
	char compressionName[256];
};

struct AIFFSoundDataChunk
{
	uint32_t offset;
	uint32_t blockSize;
};

uint32_t swapEndiannes(uint32_t i)
{
	unsigned char b1 = static_cast<unsigned char>((i & 0x000000ff) >> 0);
	unsigned char b2 = static_cast<unsigned char>((i & 0x0000ff00) >> 8);
	unsigned char b3 = static_cast<unsigned char>((i & 0x00ff0000) >> 16);
	unsigned char b4 = static_cast<unsigned char>((i & 0xff000000) >> 24);
	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

uint16_t swapEndiannes(uint16_t i)
{
	unsigned char b1 = static_cast<unsigned char>((i & 0x00ff) >> 0);
	unsigned char b2 = static_cast<unsigned char>((i & 0xff00) >> 8);
	return (b1 << 16) | b2;
}

void swapEndiannes(unsigned char* data, size_t dataSize)
{
	uint32_t* ptr = reinterpret_cast<uint32_t*>(data);
	for (size_t i = 0; i < dataSize / 4; ++i)
		*ptr++ = swapEndiannes(*ptr);
}

#pragma pack()

size_t parseOpenALFormat(size_t numChannels, size_t bitDepth)
{
	if (numChannels == 1)
	{
		if (bitDepth == 8)
		{
			return AL_FORMAT_MONO8;
		}
		else if (bitDepth == 16)
		{
			return AL_FORMAT_MONO16;
		}
		else 
		{
			assert(0 && "Unsupported format in WAV file");
		}
	}
	else if (numChannels == 2)
	{
		if (bitDepth == 8)
		{
			return AL_FORMAT_STEREO8;
		}
		else if (bitDepth == 16)
		{
			return AL_FORMAT_STEREO16;
		}
		else 
		{
			assert(0 && "Unsupported format in WAV file");
		}
	}
	else 
	{
		assert(0 && "Unsupported number of channels in WAV file");
	}

	return 0;
}

#define UnsignedToFloat(u) (((double) ((long) (u - 2147483647L - 1))) + 2147483648.0)
double _af_convert_from_ieee_extended (const unsigned char *bytes)
{
	double			f;
	int				expon;
	unsigned long	hiMant, loMant;

	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);

	hiMant = ((unsigned long)(bytes[2] & 0xFF) << 24)
		| ((unsigned long) (bytes[3] & 0xFF) << 16)
		| ((unsigned long) (bytes[4] & 0xFF) << 8)
		| ((unsigned long) (bytes[5] & 0xFF));

	loMant = ((unsigned long) (bytes[6] & 0xFF) << 24)
		| ((unsigned long) (bytes[7] & 0xFF) << 16)
		| ((unsigned long) (bytes[8] & 0xFF) << 8)
		| ((unsigned long) (bytes[9] & 0xFF));

	if ((expon == 0) && (hiMant == 0) && (loMant == 0))
	{
		f = 0;
	}
	else {
		if (expon == 0x7FFF) {	  /* Infinity or NaN */
			f = HUGE_VAL;
		}
		else {
			expon -= 16383;
			f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
			f += ldexp(UnsignedToFloat(loMant), expon-=32);
		}
	}

	if (bytes[0] & 0x80)
		return -f;
	else
		return f;
}

Description::Pointer et::audio::loadWAVFile(const std::string& fileName)
{
	std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);
	if (file.fail())
		return Description::Pointer();

	WAVFileChunk fileChunk = { };
	file.read(reinterpret_cast<char*>(&fileChunk), sizeof(fileChunk));
	if (fileChunk.id.nID != WAVFileChunkID)
		return Description::Pointer();

	Description* result = 0;

	while (!file.eof() && file.good())
	{
		AudioFileChunk chunk = { };
		file.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));

		if (chunk.id.nID == WAVFormatChunkID)
		{
			WAVFormatChunk fmt = { };
			file.read(reinterpret_cast<char*>(&fmt), sizeof(fmt));

			result = new Description;
			result->source = fileName;
			result->sampleRate = fmt.sampleRate;
			result->channels = fmt.numChannels;
			result->bitDepth = fmt.bitsPerSample;
			result->format = parseOpenALFormat(result->channels, result->bitDepth);
		}
		else if ((chunk.id.nID == WAVDataChunkID) && (result != nullptr))
		{
			result->data.resize(chunk.size);
			result->duration = static_cast<float>(result->data.dataSize()) / (
				static_cast<float>(result->sampleRate * result->channels * result->bitDepth / 8));
			file.read(result->data.binary(), chunk.size);
		}
		else if (chunk.size > 0)
		{
			BinaryDataStorage data(chunk.size);
			file.read(data.binary(), chunk.size);
		}
		else 
		{
			break;
		}
	}

	return Description::Pointer(result);
}

Description::Pointer et::audio::loadCAFFile(const std::string& fileName)
{
	assert("CAF is not currently supported");
	return Description::Pointer();
}

Description::Pointer et::audio::loadAIFFile(const std::string& fileName)
{
	std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);
	if (file.fail())
		return Description::Pointer();

	AIFFFileChunk header = { };
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	header.ckDataSize = swapEndiannes(header.ckDataSize);

	Description* result = 0;

	while (!file.eof() && !file.fail())
	{
		AudioFileChunk chunk = { };
		file.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));
		chunk.size = swapEndiannes(chunk.size);

		if (chunk.id.nID == AIFFCommonChunkID)
		{
			AIFFCommonChunk comm = { };
			file.read(reinterpret_cast<char*>(&comm), chunk.size);

			result = new Description;
			result->source = fileName;
			result->bitDepth = swapEndiannes(comm.sampleSize);
			result->channels = swapEndiannes(comm.numChannels);
			result->sampleRate = static_cast<size_t>(_af_convert_from_ieee_extended(comm.c_sampleRate));
			result->format = parseOpenALFormat(result->channels, result->bitDepth);
		}
		else if ((chunk.id.nID == AIFFUncompressedDataChunkID) && (result != nullptr))
		{
			AIFFSoundDataChunk ssnd = { };
			file.read(reinterpret_cast<char*>(&ssnd), sizeof(ssnd));
			file.seekg(ssnd.offset, std::ios::cur);

			result->data.resize(chunk.size - sizeof(ssnd));
			result->duration = static_cast<float>(result->data.dataSize()) / 
				static_cast<float>((result->sampleRate * result->channels * result->bitDepth / 8));
			file.read(result->data.binary(), result->data.dataSize());

			swapEndiannes(result->data.data(), result->data.dataSize());
		}
		else if (chunk.id.nID == AIFFCompressedDataChunkID)
		{
			assert(0 && "Compressed audio data is not supported");
		}
		else if (chunk.size > 0)
		{
			BinaryDataStorage data(chunk.size);
			file.read(data.binary(), chunk.size);
		}
		else 
		{
			break;
		}
	}

	return Description::Pointer(result);
}

Description::Pointer et::audio::loadFile(const std::string& fileName)
{
	size_t dotPos = fileName.rfind('.');
	if (dotPos == std::string::npos)
		return Description::Pointer();

	std::string ext = fileName.substr(dotPos);
	lowercase(ext);

	if (ext == ".wav")
	{
		return loadWAVFile(fileName);
	}
	else if (ext == ".caf")
	{
		return loadCAFFile(fileName);
	}
	else if ((ext == ".aif") || (ext == ".aiff") || (ext == ".aifc"))
	{
		return loadAIFFile(fileName);
	}

	return Description::Pointer();
}
