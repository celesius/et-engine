/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/debug.h>

namespace et
{
	template <typename T>
	struct ContainerBase
	{
		static const size_t typeSize()
			{ return sizeof(T); }
	};
}
