/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/programdata.h>

namespace et
{
	class Program : public IntrusivePtr<ProgramData>
	{
	public:
		Program() :
			IntrusivePtr<ProgramData>(0) { }

	private:
		friend class ProgramFactory;

		Program(ProgramData* data) : 
			IntrusivePtr<ProgramData>(data) { }
	};
}
