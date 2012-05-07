/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <libpng/png.h>
#include <et/imaging/imagewriter.h>

using namespace et;

bool internal_writePNG(const std::string& fileName, const BinaryDataStorage& data, const vec2i& size, int components, int bitsPerComponent)
{
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		return false;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}
/*
	if (setjmp(png_jmpbuf(png_ptr)))
		return false;
*/
	FILE* fp = fopen(fileName.c_str(), "wb");
	if (!fp) 
		return false; 

	png_init_io(png_ptr, fp);

	png_byte colorType = 0;
	switch (components)
	{
	case 3: 
		{
			colorType = PNG_COLOR_TYPE_RGB;
			break;
		}
	case 4: 
		{
			colorType = PNG_COLOR_TYPE_RGBA;
			break;
		}
	}

	png_set_IHDR(png_ptr, info_ptr, size.x, size.y, bitsPerComponent, colorType, 
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
/*
	if (setjmp(png_jmpbuf(png_ptr)))
		return false;
*/
	png_bytep* row_pointers = new png_bytep[size.y];

	int rowSize = size.x * components * bitsPerComponent / 8;

	for (int y = 0; y < size.y; y++)
		row_pointers[y] = (png_bytep)(&data[(size.y - 1 - y) * rowSize]);

	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);

	delete [] row_pointers;

	return true;
}

bool internal_writeRAW(const std::string&, const BinaryDataStorage&, const vec2i&, int, int)
{
	return false;
}

bool ImageWriter::writeImageToFile(const std::string& fileName, const BinaryDataStorage& data, 
	const vec2i& size, int components, int bitsPerComponent, ImageFormat fmt)
{
	switch (fmt)
	{
	case ImageFormat_PNG:
		return internal_writePNG(fileName, data, size, components, bitsPerComponent);

	case ImageFormat_RAW:
		return internal_writeRAW(fileName, data, size, components, bitsPerComponent);

	default:
		return false;
	}
}

std::string ImageWriter::extensionForImageFormat(ImageFormat fmt)
{
	switch (fmt)
	{
	case ImageFormat_PNG:
		return ".png";

	case ImageFormat_RAW:
		return ".raw";

	default:
		return ".unrecognizedimageformat";
	}
}