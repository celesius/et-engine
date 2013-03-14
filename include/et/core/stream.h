/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <iosfwd>
#include <et/core/intrusiveptr.h>

namespace et
{
	enum StreamMode
	{
		StreamMode_Text,
		StreamMode_Binary
	};

	class InputStreamPrivate;
	class InputStream : public Shared
	{
	public:
		typedef IntrusivePtr<InputStream> Pointer;
		
	public:
		InputStream();
		InputStream(const std::string& file, StreamMode mode);

		~InputStream();

		bool valid();
		bool invalid();

		std::istream& stream();

	private:
		InputStreamPrivate* _private;
	};
}