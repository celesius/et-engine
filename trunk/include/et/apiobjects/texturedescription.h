/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/tools.h>
#include <et/core/containers.h>
#include <et/geometry/geometry.h>

namespace et
{
	enum TextureOrigin
	{
		TextureOrigin_TopLeft,
		TextureOrigin_BottomLeft
	};

	class TextureDescription : public Shared
	{  
	public:
		typedef IntrusivePtr<TextureDescription> Pointer;
		typedef std::vector<TextureDescription::Pointer> List;

	public:
		TextureDescription() : size(0), target(0), internalformat(0), format(0), type(0), compressed(0),
			bitsPerPixel(0), channels(0), mipMapCount(0), layersCount(0) { }

		vec2i sizeForMipLevel(size_t level)
			{ return size / intPower(2, level); }

		size_t dataSizeForMipLevel(size_t level)
		{
			size_t actualSize = sizeForMipLevel(level).square() * bitsPerPixel / 8;
			return compressed ? etMax(static_cast<size_t>(32), actualSize) : actualSize;
		}

		size_t dataSizeForAllMipLevels()
		{
			size_t result = 0;
			for (size_t i = 0; i < mipMapCount; ++i)
				result += dataSizeForMipLevel(i);
			return result;
		}

		size_t dataOffsetForLayer(size_t layer)
		{
			return dataSizeForAllMipLevels() * ((layer < layersCount) ? layer : (layersCount > 0 ? layersCount - 1 : 0));
		}

		size_t dataOffsetForMipLevel(size_t level, size_t layer = 0)
		{
			size_t result = dataOffsetForLayer(layer);
			for (size_t i = 0; i < level; ++i)
				result += dataSizeForMipLevel(i);
			return result;
		}
		
		bool valid() const
			{ return internalformat && format && (size.square() > 0); }

	private:
		TextureDescription(const TextureDescription&)
			{ }

		TextureDescription& operator = (const TextureDescription&)
			{ return *this; }

	public:
		std::string source;
		
		BinaryDataStorage data;
		vec2i size;
		
		int target;
		int internalformat;
		int format;
		int type;
		int compressed;
		
		size_t bitsPerPixel;
		size_t channels;
		size_t mipMapCount;
		size_t layersCount;
	};

}