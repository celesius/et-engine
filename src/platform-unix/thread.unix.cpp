/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
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
		ThreadPrivate() :
			threadId(0) { }
		
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

ThreadPrivate::~ThreadPrivate()
{
	pthread_mutex_destroy(&suspendMutex);
	pthread_cond_destroy(&suspend);
}

void* ThreadPrivate::threadProc(void* context)
{
	Thread* thread = static_cast<Thread*>(context);
	thread->_private->threadId = reinterpret_cast<ThreadId>(pthread_self());
	int result = thread->main();
	thread->terminate(result);
	return reinterpret_cast<void*>(result);
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

void Thread::run()
{
	if (_private->running.atomicCounterValue() > 0) return;
	
	pthread_mutex_init(&_private->suspendMutex, 0);
	pthread_cond_init(&_private->suspend, 0);
	
	pthread_attr_t attrib = { };
	pthread_attr_init(&attrib);
	pthread_attr_setdetachstate(&attrib, PTHREAD_CREATE_DETACHED);
	
	_private->running.retain();
	pthread_create(&_private->thread, &attrib, ThreadPrivate::threadProc, this);
	
	pthread_attr_destroy(&attrib);
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

void Thread::terminate(int result)
{
	if (_private->running.atomicCounterValue() == 0) return;
	
	_private->running.release();
	
	pthread_detach(_private->thread);
	pthread_exit(reinterpret_cast<void*>(result));
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
