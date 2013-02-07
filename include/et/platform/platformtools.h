/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/et.h>

namespace et
{
	inline bool platformHasHardwareKeyboard()
		{ return CurrentPlatform != Platform_iOS; }

	enum SelectFileMode
	{
		SelectFileMode_Open,
		SelectFileMode_Save
	};

	std::string selectFile(const StringList& types, SelectFileMode mode);
}
