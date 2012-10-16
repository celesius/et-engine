/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/containers.h>

namespace et
{
	namespace audio
	{
		class Description : public Shared
		{
		public:
			typedef IntrusivePtr<Description> Pointer;
			
		public:
			Description() :	
				format(0), channels(0), bitDepth(0), sampleRate(0) { }

		public:
			std::string source;

			size_t format;
			size_t channels;
			size_t bitDepth;
			size_t sampleRate;

			BinaryDataStorage data;
		};
	};
}