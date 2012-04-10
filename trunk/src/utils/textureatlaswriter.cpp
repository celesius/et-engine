#include <et/utils/textureatlaswriter.h>
#include <et/utils/imagewriter.h>
#include <et/loaders/pngloader.h>
#include <et/utils/imageoperations.h>

using namespace et;
using namespace et::gui;

TextureAtlasWriter::TextureAtlasItem& TextureAtlasWriter::addItem(const vec2i& textureSize)
{
	_items.push_back(TextureAtlasItem());
	TextureAtlasItem& item = _items.back();
	item.texture.size = textureSize;
	return item;
}

bool TextureAtlasWriter::placeImage(const TextureDescription& image, TextureAtlasItem& item)
{
	int w = image.size.x;
	int h = image.size.y;
	int xOffset = 0;
	int yOffset = 0;

	if (_addSpace)
	{
		if (w < item.texture.size.x - 1)
		{
			w++;
			xOffset = 1;
		}

		if (h < item.texture.size.y - 1)
		{
			h++;
			yOffset = 1;
		}
	}

	vec2 size(static_cast<float>(w), static_cast<float>(h));
	gui::ImageDescriptor desc(vec2(0.0f), size);

	if (item.images.size() == 0)
	{
		item.images.push_back(ImageItem(image, desc));

		if (desc.origin.x + desc.size.x > item.maxWidth) 
			item.maxWidth = static_cast<int>(desc.origin.x + desc.size.x) - xOffset;

		if (desc.origin.y + desc.size.y > item.maxHeight) 
			item.maxHeight = static_cast<int>(desc.origin.y + desc.size.y) - yOffset;

		return true;
	}
	
	for (ImageItemList::iterator i = item.images.begin(), e = item.images.end(); i != e; ++i)
	{
		desc.origin = i->place.origin + vec2(i->place.size.x, 0.0f);

		bool placed = (desc.origin.x + w <= item.texture.size.x) && (desc.origin.y + h <= item.texture.size.y);
		if (placed)
		{
			for (ImageItemList::iterator ii = item.images.begin(); ii != e; ++ii)
			{
				if ((ii != i) && ii->place.rectangle().intersects(desc.rectangle()))
				{
					placed = false;
					break;
				}
			}
		}

		if (placed)
		{
			item.images.push_back(ImageItem(image, desc));

			if (desc.origin.x + desc.size.x > item.maxWidth) 
				item.maxWidth = static_cast<int>(desc.origin.x + desc.size.x) - xOffset;

			if (desc.origin.y + desc.size.y > item.maxHeight) 
				item.maxHeight = static_cast<int>(desc.origin.y + desc.size.y) - yOffset;
			
			return true;
		}

		desc.origin = i->place.origin + vec2(0.0f, i->place.size.y);
		placed = (desc.origin.x + w <= item.texture.size.x) && (desc.origin.y + h <= item.texture.size.y);
		if (placed)
		{
			for (ImageItemList::iterator ii = item.images.begin(); ii != e; ++ii)
			{
				if ((ii != i) && ii->place.rectangle().intersects(desc.rectangle()))
				{
					placed = false;
					break;
				}
			}
		}

		if (placed)
		{
			item.images.push_back(ImageItem(image, desc));

			if (desc.origin.x + desc.size.x > item.maxWidth) 
				item.maxWidth = static_cast<int>(desc.origin.x + desc.size.x) - xOffset;

			if (desc.origin.y + desc.size.y > item.maxHeight) 
				item.maxHeight = static_cast<int>(desc.origin.y + desc.size.y) - yOffset;

			return true;
		}
	}

	return false;
}

inline bool textureNameSort(const TextureAtlasWriter::ImageItem& i1, const TextureAtlasWriter::ImageItem& i2)
{
	return i1.image.source < i2.image.source;
}

void TextureAtlasWriter::writeToFile(const std::string& fileName, const char* textureNamePattern)
{
	std::string path = addTrailingSlash(getFilePath(fileName));
	std::ofstream descFile(fileName.c_str());

	int textureIndex = 0;
	for (TextureAtlasItemList::iterator i = _items.begin(), e = _items.end(); i != e; ++i, ++textureIndex)
	{
		BinaryDataStorage data(i->texture.size.square() * 4);
		data.fill(0);

		BinaryDataStorage layoutData(i->texture.size.square() * 4);
		layoutData.fill(0);

		char textureName[1024] = { };
		sprintf(textureName, textureNamePattern, textureIndex);
		std::string texName = path + std::string(textureName);
		descFile << "texture: " << textureName << std::endl;

		int index = 0;
		for (ImageItemList::iterator ii = i->images.begin(), ie = i->images.end(); ii != ie; ++ii, ++index)
		{
			TextureDescription image;
			PNGLoader::loadFromFile(ii->image.source, image);

			vec2i iOrigin(static_cast<int>(ii->place.origin.x), static_cast<int>(ii->place.origin.y));

			ii->place.size = vec2(static_cast<float>(i->texture.size.x), static_cast<float>(i->texture.size.y));

			ImageOperations::fill(layoutData, i->texture.size, 4, recti(iOrigin, image.size),
				vec4ub(rand() % 256, rand() % 256, rand() % 256, 255));

			std::string sIndex = intToStr(index);
			if (sIndex.length() < 2)
				sIndex = "0" + sIndex;
			std::string newFile = replaceFileExt(texName, ".layout" + sIndex + ".png");
			std::string name = removeFileExt(getFileName(ii->image.source));

			vec4 offset;
			size_t delimPos = name.find_first_of("~");
			if (delimPos != std::string::npos)
			{
				std::string params = name.substr(delimPos + 1);
				name.erase(delimPos);
				while (params.length())
				{
					size_t dPos = params.find_first_of("-");
					if (dPos == std::string::npos)
					{
						std::cout << "Unable to parse all image parameters: `" << params << "`" << std::endl;
						break;
					}

					std::string token = params.substr(0, dPos+1);
					params.erase(0, dPos+1);
					if (token == "offset-")
					{
						offset = strToVec4(params);
					}
					else 
					{
						std::cout << "Unrecognized token: " << token << std::endl;
						break;
					}
				}
			}

			descFile << "image: { name: \"" << name << "\" "
						"texture: \"" << textureName << "\"" << " "
						"rect: \"" << iOrigin << ";" << image.size << "\" "
						"offset: \""  << offset <<  "\" }" << std::endl;

			int components = 0;
			switch (image.format)
			{
			case GL_RGB:
				{
					components = 3;
					break;
				}
			case GL_RGBA:
				{
					components = 4;
					break;
				}
			default:
				break;
			};

			if (components)
				ImageOperations::transfer(image.data, image.size, components, data, i->texture.size, 4, iOrigin);
		}

		ImageWriter::writeImageToFile(replaceFileExt(texName, ".layout.png"), layoutData, i->texture.size, 4, 8, ImageFormat_PNG);
		ImageWriter::writeImageToFile(texName, data, i->texture.size, 4, 8, ImageFormat_PNG);
	}
}

