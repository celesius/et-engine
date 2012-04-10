#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/programdata.h>

namespace et
{

	class Program : public IntrusivePtr<ProgramData>
	{
	private:
		friend class ProgramFactory;
		Program(ProgramData* data) : IntrusivePtr<ProgramData>(data) { }

	public:
		Program() : IntrusivePtr<ProgramData>(0) { }

	};

}