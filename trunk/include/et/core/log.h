/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

namespace et
{
	namespace log
	{
		void debug(const char*, ...);
		void info(const char*, ...);
		void warning(const char*, ...);
		void error(const char*, ...);

		void debug(const wchar_t*, ...);
		void info(const wchar_t*, ...);
		void warning(const wchar_t*, ...);
		void error(const wchar_t*, ...);
	}
}