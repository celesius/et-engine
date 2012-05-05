#include <iostream>
#include <pthread.h>
#include <libkern/OSAtomic.h>
#include <et/core/tools.h>
#include <et/threading/thread.h>

namespace et
{
	class ThreadPrivate
	{
	public:
		ThreadPrivate() : threadId(0), running(0), suspended(0) { }
		~ThreadPrivate();
		static void* threadProc(void* context);
		
	public:
		pthread_t thread;
		pthread_mutex_t suspendMutex;
		pthread_cond_t suspend;
		ThreadId threadId;
		volatile int running;
		volatile int suspended;
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
	if (_private->running) return;
	
	pthread_mutex_init(&_private->suspendMutex, 0);
	pthread_cond_init(&_private->suspend, 0);
	
	pthread_attr_t attrib = { };
	pthread_attr_init(&attrib);
	pthread_attr_setdetachstate(&attrib, PTHREAD_CREATE_DETACHED);
	
	OSAtomicIncrement32(&_private->running);
	pthread_create(&_private->thread, &attrib, ThreadPrivate::threadProc, this);
	
	pthread_attr_destroy(&attrib);
}

void Thread::suspend()
{
	if (_private->suspended) return;
	
	OSAtomicIncrement32(&_private->suspended);
	
	pthread_mutex_lock(&_private->suspendMutex);
	pthread_cond_wait(&_private->suspend, &_private->suspendMutex);
	pthread_mutex_unlock(&_private->suspendMutex);
}

void Thread::resume()
{
	if (!_private->suspended) return;
	
	OSAtomicDecrement32(&_private->suspended);
	
	pthread_mutex_lock(&_private->suspendMutex);
	pthread_cond_signal(&_private->suspend);
	pthread_mutex_unlock(&_private->suspendMutex);
}

void Thread::terminate(int result)
{
	if (!_private->running) return;
	
	OSAtomicDecrement32(&_private->running);
	
	pthread_detach(_private->thread);
	pthread_exit(reinterpret_cast<void*>(result));
}

ThreadResult Thread::main()
{
	return 0;
}

bool Thread::running() const
{
	return _private->running;
}

bool Thread::suspended() const
{
	return _private->suspended;
}

ThreadId Thread::id() const
{
	return _private->threadId;
}
