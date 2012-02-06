#pragma once

#include <et/apiobjects/texture.h>
#include <et/gui/guibase.h>

namespace et
{
	class TextureAtlasWriter
	{
	public:
		struct ImageItem
		{
			TextureDescription image;
			gui::ImageDescriptor place;

			ImageItem(const TextureDescription& t, const gui::ImageDescriptor& p) : image(t), place(p) { }
		};

		typedef std::vector<ImageItem> ImageItemList;

		struct TextureAtlasItem
		{
			TextureDescription texture;
			ImageItemList images;
			int maxWidth;
			int maxHeight;
			TextureAtlasItem() : maxWidth(0), maxHeight(0) { }
		};

		typedef std::vector<TextureAtlasItem> TextureAtlasItemList;

	public:
		TextureAtlasWriter(bool addSpace = true) : _addSpace(addSpace) { }
		TextureAtlasItem& addItem(const vec2i& textureSize);
		bool placeImage(const TextureDescription& image, TextureAtlasItem& item);

		inline const TextureAtlasItemList& items() const 
			{ return _items; }

		void writeToFile(const std::string& fileName, const char* textureNamePattern = "texture_%d.png");

	private:
		TextureAtlasItemList _items;
		bool _addSpace;
	};

}