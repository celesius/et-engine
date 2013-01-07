/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/threading/threading.h>
#include <sys/sysctl.h>
#include <pthread.h>

using namespace et;

ThreadId Threading::_mainThread;
ThreadId Threading::_renderingThread;

Threading::Threading()
{
	_mainThread = reinterpret_cast<ThreadId>(pthread_self());
	_renderingThread = _mainThread;
}

unsigned long Threading::currentThread()
{
	return reinterpret_cast<ThreadId>(pthread_self());
}

void Threading::setMainThread(ThreadId t)
{
	_mainThread = t;
}

void Threading::setRenderingThread(ThreadId t)
{
	_renderingThread = t;
}

size_t Threading::coresCount()
{
	size_t numCPU = 1;
	
	int mib[4] = { };
	size_t len = sizeof(numCPU); 
	
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;
	sysctl(mib, 2, &numCPU, &len, NULL, 0);
	
	if (numCPU < 1) 
	{
		mib[1] = HW_NCPU;
		sysctl( mib, 2, &numCPU, &len, NULL, 0 );
		
		if (numCPU < 1)
			numCPU = 1;
	}
	
	return numCPU;
}

double Threading::cpuUsage()
{
	return 0.0;
}