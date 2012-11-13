/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <libjpeg/jpeglib.h>

#include <et/core/tools.h>
#include <et/opengl/opengl.h>
#include <et/imaging/jpgloader.h>

using namespace et;

void JPGLoader::loadInfoFromStream(std::istream& stream, TextureDescription& desc)
{
	if (stream.fail()) return;
	
	BinaryDataStorage buffer(streamSize(stream));
	stream.read(reinterpret_cast<char*>(buffer.data()), buffer.dataSize());
	
	jpeg_decompress_struct cinfo = { };
	jpeg_error_mgr jerr = { };
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, buffer.data(), buffer.dataSize());
	jpeg_read_header(&cinfo, true);
	if (cinfo.out_color_space == JCS_GRAYSCALE)
	{
		jpeg_destroy_decompress(&cinfo);
		return;
	}

	desc.size = vec2i(cinfo.image_width, cinfo.image_height);
	desc.target = GL_TEXTURE_2D;
	desc.internalformat = GL_RGB;
	desc.format = GL_RGB;
	desc.type = GL_UNSIGNED_BYTE;
	desc.compressed = 0;
	desc.bitsPerPixel = 24;
	desc.channels = 3;
	desc.mipMapCount = 1;
	desc.layersCount = 1;
	
	jpeg_destroy_decompress(&cinfo);
}

void JPGLoader::loadFromStream(std::istream& stream, TextureDescription& desc)
{
	if (stream.fail()) return;
	
	BinaryDataStorage buffer(streamSize(stream));
	stream.read(reinterpret_cast<char*>(buffer.data()), buffer.dataSize());

	jpeg_decompress_struct cinfo = { };
	jpeg_error_mgr jerr = { };
	
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, buffer.data(), buffer.dataSize());
	jpeg_read_header(&cinfo, true);
	jpeg_calc_output_dimensions(&cinfo);
	
	if (cinfo.out_color_space == JCS_GRAYSCALE)
	{
		jpeg_destroy_decompress(&cinfo);
		return;
	}
	
	desc.size = vec2i(cinfo.output_width, cinfo.output_height);
	desc.target = GL_TEXTURE_2D;
	desc.internalformat = GL_RGB;
	desc.format = GL_RGB;
	desc.type = GL_UNSIGNED_BYTE;
	desc.compressed = 0;
	desc.bitsPerPixel = 24;
	desc.channels = 3;
	desc.mipMapCount = 1;
	desc.layersCount = 1;
	desc.data = BinaryDataStorage(desc.size.square() * cinfo.output_components);
	
	size_t rowSize = cinfo.output_components * cinfo.output_height;
	unsigned char* p_line = desc.data.data();
	
	if (jpeg_start_decompress(&cinfo))
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			int linesRead = jpeg_read_scanlines(&cinfo, &p_line, 1);
			p_line += linesRead * rowSize;
		}
	}
	else
	{
		std::cout << "Unable to decompress JPEG" << std::endl;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}

void JPGLoader::loadInfoFromFile(const std::string& path, TextureDescription& desc)
{
	std::ifstream stream(path.c_str());
	desc.source = path;
	loadInfoFromStream(stream, desc);
}

void JPGLoader::loadFromFile(const std::string& path, TextureDescription& desc)
{
	std::ifstream stream(path.c_str());
	desc.source = path;
	loadFromStream(stream, desc);
}