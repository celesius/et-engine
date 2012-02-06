#pragma once

#include <sys/types.h>
#include <et/core/singleton.h>
#include <et/threading/thread.h>

namespace et
{
	class Threading : public Singleton<Threading>
	{
	public:
		static size_t coresCount();
		static double cpuUsage();
		static ThreadId currentThread();
		static ThreadId mainThread() 
			{ return _mainThread; }

	private:
		Threading();
		ET_SINGLETON_COPY_DENY(Threading)

	private:
		static ThreadId _mainThread;
	};

	inline Threading& threading() { return Threading::instance(); }
}