#pragma once

#include <et/platform/platform.h>

namespace et
{
#if (ET_PLATFORM_IOS || ET_PLATFORM_MAC)
	typedef int ReferenceCounterType;
#elif (ET_PLATFORM_WIN)
	typedef long ReferenceCounterType;
#else
	#error ReferenceCounterType isn't defined
#endif
	
	class ReferenceCounter
	{
	public:
		ReferenceCounter();
		virtual ~ReferenceCounter() { }

		ReferenceCounterType retain();
		ReferenceCounterType release();

		inline ReferenceCounterType referenceCount()
			{ return _referenceCount; }

	private:
		volatile ReferenceCounterType _referenceCount;
	};
}