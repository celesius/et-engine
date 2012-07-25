/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/geometry/geometry.h>

namespace et
{
	class PixelFilter
	{
	public:
		virtual void applyRGBA(vec4ub& pixel, void* context) = 0;
	};

	enum ImageBlendType
	{
		ImageBlendType_Default,
		ImageBlendType_Additive
	};

	enum ImageBlurType
	{
		ImageBlurType_Average,
		ImageBlurType_Linear
	};

	enum ImageFilteringType
	{
		ImageFilteringType_Nearest,
		ImageFilteringType_Linear
	};

	class ImageOperations
	{
	public:
		static mat3i matrixFilterBlur;
		static mat3i matrixFilterSharpen;
		static mat3i matrixFilterStrongBlur;

		static void transfer(const BinaryDataStorage& src, const vec2i& srcSize, size_t srcComponents,
			BinaryDataStorage& dst, const vec2i& dstSize, size_t dstComponents, const vec2i& position);

		static void draw(const BinaryDataStorage& src, const vec2i& srcSize, size_t srcComponents,
			BinaryDataStorage& dst, const vec2i& dstSize, size_t dstComponents, const recti& destRect,
			ImageBlendType blend, ImageFilteringType filter);

		static void fill(BinaryDataStorage& dst, const vec2i& dstSize, size_t dstComponents, const recti& r, const vec4ub& color);

		static void applyPixelFilter(BinaryDataStorage& data, const vec2i& size, size_t components, PixelFilter* filter, void* context);
		static void applyMatrixFilter(BinaryDataStorage& data, const vec2i& size, size_t components, const mat3i& m);

		static void blur(BinaryDataStorage& data, const vec2i& size, size_t components, vec2i direction, size_t radius, ImageBlurType type);
		static void median(BinaryDataStorage& data, const vec2i& size, size_t components, int radius);

		static void normalMapFilter(BinaryDataStorage& data, const vec2i& size, size_t components, const vec2& scale);

	};
}