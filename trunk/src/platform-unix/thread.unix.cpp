/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <pthread.h>
#include <unistd.h>
#include <et/threading/atomiccounter.h>
#include <et/threading/thread.h>

namespace et
{
	class ThreadPrivate
	{
	public:
		ThreadPrivate();
		~ThreadPrivate();
		
		static void* threadProc(void* context);
		
	public:
		pthread_t thread;
		pthread_mutex_t suspendMutex;
		pthread_cond_t suspend;
		ThreadId threadId;
		AtomicCounter running;
		AtomicCounter suspended;
	};
}

using namespace et;

ThreadPrivate::ThreadPrivate() : thread(0), threadId(0)
{
	suspendMutex = { };
	suspend = { };
}

ThreadPrivate::~ThreadPrivate()
{
	pthread_mutex_destroy(&suspendMutex);
	pthread_cond_destroy(&suspend);
}

void* ThreadPrivate::threadProc(void* context)
{
	Thread* thread = static_cast<Thread*>(context);
	thread->_private->threadId = reinterpret_cast<ThreadId>(pthread_self());

	return reinterpret_cast<void*>(thread->main());
}

Thread::Thread(bool runImmediately) : _private(new ThreadPrivate)
{
	if (runImmediately)
		run();
}

Thread::~Thread()
{
	delete _private;
}

bool Thread::run()
{
	if (_private->running.atomicCounterValue() > 0) return false;
	
	pthread_mutex_init(&_private->suspendMutex, 0);
	pthread_cond_init(&_private->suspend, 0);
	
	pthread_attr_t attrib = { };
	pthread_attr_init(&attrib);
	pthread_attr_setdetachstate(&attrib, PTHREAD_CREATE_DETACHED);
	
	_private->running.retain();
	
	pthread_create(&_private->thread, &attrib, ThreadPrivate::threadProc, this);
	pthread_join(_private->thread, nullptr);

	pthread_attr_destroy(&attrib);
	return true;
}

void Thread::suspend()
{
	if (_private->suspended.atomicCounterValue() > 0) return;
	
	_private->suspended.retain();
	
	pthread_mutex_lock(&_private->suspendMutex);
	pthread_cond_wait(&_private->suspend, &_private->suspendMutex);
	pthread_mutex_unlock(&_private->suspendMutex);
}

void Thread::resume()
{
	if (_private->suspended.atomicCounterValue() == 0) return;
	
	_private->suspended.release();
	
	pthread_mutex_lock(&_private->suspendMutex);
	pthread_cond_signal(&_private->suspend);
	pthread_mutex_unlock(&_private->suspendMutex);
}

bool Thread::stop()
{
	if (_private->running.atomicCounterValue() == 0) return false;
	_private->running.release();

	return true;
}

void Thread::terminate(int result)
{
	if (stop())
	{
		pthread_detach(_private->thread);
		pthread_exit(reinterpret_cast<void*>(result));
	}
}

ThreadResult Thread::main()
{
	return 0;
}

bool Thread::running() const
{
	return _private->running.atomicCounterValue() > 0;
}

bool Thread::suspended() const
{
	return _private->suspended.atomicCounterValue() > 0;
}

ThreadId Thread::id() const
{
	return _private->threadId;
}

void Thread::sleep(float seconds)
{
	usleep(static_cast<useconds_t>(seconds * 1000000.0f));
}

void Thread::sleepMSec(uint64_t msec)
{
	usleep(static_cast<useconds_t>(msec) * 1000);
}
