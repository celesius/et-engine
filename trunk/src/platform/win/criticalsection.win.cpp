#include <Windows.h>
#include <iostream>
#include <et/threading/criticalsection.h>

namespace et
{
	class CriticalSectionPrivate
	{
	public:
		CriticalSectionPrivate() 
			{ InitializeCriticalSection(&_cs); }

		~CriticalSectionPrivate()
			{ DeleteCriticalSection(&_cs); }

		void enter()
			{ EnterCriticalSection(&_cs); }

		void leave()
			{ LeaveCriticalSection(&_cs); }

	private:
		RTL_CRITICAL_SECTION _cs;
	};
};

using namespace et;

CriticalSection::CriticalSection() : _private(new CriticalSectionPrivate)
{
}

CriticalSection::~CriticalSection() 
{
	delete _private;
}

void CriticalSection::enter()
{
	_private->enter();
}

void CriticalSection::leave()
{
	_private->leave();
}