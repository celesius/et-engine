#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/programdata.h>

namespace et
{

	class Program : public IntrusivePtr<ProgramData>
	{
	private:
		friend class ProgramFactory;
		inline Program(ProgramData* data) : IntrusivePtr<ProgramData>(data) { }

	public:
		inline Program() : IntrusivePtr<ProgramData>(0) { }

	};

}