/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <libpng/png.h>

#include <et/opengl/opengl.h>
#include <et/imaging/pngloader.h>

using namespace et;

void parseFormat(TextureDescription& desc, png_structp pngPtr, png_infop infoPtr, png_size_t* rowBytes);
void userReadData(png_structp pngPtr, png_bytep data, png_size_t length);

void PNGLoader::loadInfoFromStream(std::istream& source, TextureDescription& desc)
{
	static const int PNGSIGSIZE = 8;

	png_byte pngsig[PNGSIGSIZE] = { };
	source.read((char*)pngsig, PNGSIGSIZE);

	if (png_sig_cmp(pngsig, 0, PNGSIGSIZE))
	{
		std::cout << "ERROR: Couldn't recognize PNG" << std::endl;    
		return;
	}

	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngPtr)
	{
		std::cout << "ERROR: Couldn't initialize png read struct" << std::endl;    
		return;
	}

	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr) 
	{
		std::cout << "ERROR: Couldn't initialize png info struct" << std::endl;   
		png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);    
		return;
	}

	png_set_read_fn(pngPtr, (png_voidp)&source, userReadData); 
	png_set_sig_bytes(pngPtr, PNGSIGSIZE);
	png_read_info(pngPtr, infoPtr); 
	parseFormat(desc, pngPtr, infoPtr, 0);
	png_destroy_info_struct(pngPtr, &infoPtr);
	png_destroy_read_struct(&pngPtr, 0, 0);
}

void PNGLoader::loadFromStream(std::istream& source, TextureDescription& desc)
{
	static const int PNGSIGSIZE = 8;

	png_byte pngsig[PNGSIGSIZE] = { };
	source.read((char*)pngsig, PNGSIGSIZE);

	if (png_sig_cmp(pngsig, 0, PNGSIGSIZE))
	{
		std::cout << "ERROR: Couldn't recognize PNG" << std::endl;    
		return;
	}

	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngPtr)
	{
		std::cout << "ERROR: Couldn't initialize png read struct" << std::endl;    
		return;
	}

	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr) 
	{
		std::cout << "ERROR: Couldn't initialize png info struct" << std::endl;   
		png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);    
		return;
	}

	png_size_t rowBytes = 0;

	png_set_read_fn(pngPtr, (png_voidp)&source, userReadData); 
	png_set_sig_bytes(pngPtr, PNGSIGSIZE);
	png_read_info(pngPtr, infoPtr); 
	parseFormat(desc, pngPtr, infoPtr, &rowBytes);

	desc.data = BinaryDataStorage(desc.size.square() * desc.bitsPerPixel / 8);
	png_bytepp rowPtrs = new png_bytep[desc.size.y]; 

	for (int i = 0; i < desc.size.y; i++) 
		rowPtrs[i] = static_cast<png_bytep>(desc.data.raw()) + (desc.size.y - 1 - i) * rowBytes;   

	png_read_image(pngPtr, rowPtrs); 

	png_destroy_info_struct(pngPtr, &infoPtr);
	png_destroy_read_struct(&pngPtr, 0, 0);

	if (desc.bitsPerPixel / desc.channels == 16)
	{
		unsigned short* data_ptr = reinterpret_cast<unsigned short*>(desc.data.raw());
		for (size_t i = 0; i < desc.data.dataSize() / 2; ++i)
		{
			unsigned short value = data_ptr[i];
			data_ptr[i] = (static_cast<unsigned char>(value >> 8)) + (static_cast<unsigned char>(value) << 8);
		}
	}

	delete [] rowPtrs;
}

void PNGLoader::loadFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios::binary);
	loadFromStream(file, desc);
}

void PNGLoader::loadInfoFromFile(const std::string& path, TextureDescription& desc)
{
	desc.source = path;
	std::ifstream file(path.c_str(), std::ios::binary);
	loadInfoFromStream(file, desc);
}

/*
 * Internal stuff
 */

void parseFormat(TextureDescription& desc, png_structp pngPtr, png_infop infoPtr, png_size_t* rowBytes)
{
	desc.mipMapCount = 1;
	desc.layersCount = 1;
	desc.size.x =  png_get_image_width(pngPtr, infoPtr);    
	desc.size.x = png_get_image_height(pngPtr, infoPtr);
	desc.channels = png_get_channels(pngPtr, infoPtr);
	
	int color_type = png_get_color_type(pngPtr, infoPtr); 
	int interlace_method = png_get_interlace_type(pngPtr, infoPtr);
	int compression = png_get_compression_type(pngPtr, infoPtr);
	int filter = png_get_filter_type(pngPtr, infoPtr);
	
	switch (color_type) 
	{        
#if ET_OPENGLES		 
		case PNG_COLOR_TYPE_GRAY:
		{
			png_set_gray_to_rgb(pngPtr);
			desc.channels = 3;                       
			break;
		}
		case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			png_set_gray_to_rgb(pngPtr);
			desc.channels = 4;                       
			break;
		}
#endif
		case PNG_COLOR_TYPE_PALETTE:
		{
			png_set_palette_to_rgb(pngPtr);           
			desc.channels = 3;                       
			break;        
		}
			
		default: 
			break;
	} 
	
	if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) 
	{        
		png_set_tRNS_to_alpha(pngPtr);        
		desc.channels += 1;
	}    
	
	int bpp = 0;
	png_read_update_info(pngPtr, infoPtr);
	png_get_IHDR(pngPtr, infoPtr, (png_uint_32p)&desc.size.x, (png_uint_32p)&desc.size.y, 
				 &bpp, &color_type, &interlace_method, &compression, &filter);
	
	desc.bitsPerPixel = desc.channels * bpp;
	desc.type = (bpp == 16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;
	
	if (rowBytes)
		*rowBytes = png_get_rowbytes(pngPtr, infoPtr);
	
	switch (desc.channels)
	{
#if defined(GL_R)
		case 1: 
		{
			desc.internalformat = (bpp == 16) ? GL_R16 : GL_R;
			desc.format = GL_R;
			break;
		};
#endif
		case 2: 
		{
			desc.internalformat = (bpp == 16) ? GL_RG16 : GL_RG;
			desc.format = GL_RG;
			break;
		}
			
		case 3: 
		{
			desc.internalformat = (bpp == 16) ? GL_RGB16 : GL_RGB8;
			desc.format = GL_RGB;
			break;
		}
			
		case 4: 
		{ 
			desc.internalformat = (bpp == 16) ? GL_RGBA16 : GL_RGBA8;
			desc.format = GL_RGBA;
			break;
		}
			
		default: 
		{
			desc.internalformat = 0;
			desc.format = 0;
		}
			
	}
}

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length)
{
	png_voidp a = png_get_io_ptr(pngPtr);
	((std::istream*)a)->read((char*)data, length);
}
