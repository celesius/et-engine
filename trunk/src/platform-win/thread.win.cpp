/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>
#include <Windows.h>
#include <et/core/tools.h>
#include <et/threading/thread.h>

namespace et
{
	class ThreadPrivate
	{
	public:
		ThreadPrivate::ThreadPrivate() : 
		  threadId(0), thread(0), activityEvent(0), running(0), suspended(0) { }

		static DWORD WINAPI threadProc(LPVOID lpParameter);

	public:
		DWORD threadId;
		HANDLE thread;
		HANDLE activityEvent;
		volatile long running;
		volatile long suspended;
	};
}

using namespace et;


DWORD WINAPI ThreadPrivate::threadProc(LPVOID lpParameter)
{
	Thread* thread = reinterpret_cast<Thread*>(lpParameter);
	InterlockedIncrement(&thread->_private->running);

	ThreadResult result = thread->main();

	thread->terminate();
	return static_cast<DWORD>(result);
}

/*
 * Thread
 */

Thread::Thread(bool runImmediately) : _private(new ThreadPrivate())
{
	_private->activityEvent = CreateEvent(0, false, false, 0);

	if (runImmediately)
		run();
}

Thread::~Thread()
{
	terminate();
	CloseHandle(_private->activityEvent);
	delete _private;
}

void Thread::run()
{
	if (_private->running) return;
	_private->thread = CreateThread(0, 0, ThreadPrivate::threadProc, this, 0, &_private->threadId);
}

ThreadResult Thread::main()
{
	return 0;
}

void Thread::suspend()
{
	if (_private->suspended) return;

	InterlockedIncrement(&_private->suspended);
	WaitForSingleObject(_private->activityEvent, INFINITE);
}

void Thread::resume()
{
	if (_private->suspended == 0) return;

	InterlockedDecrement(&_private->suspended);
	SetEvent(_private->activityEvent);
}

void Thread::terminate(int exitCode)
{
	if (_private->running == 0) return;

	DWORD threadExitCode = 0;
	GetExitCodeThread(_private->thread, &threadExitCode);

	if (threadExitCode == STILL_ACTIVE)
		TerminateThread(_private->thread, exitCode);

	CloseHandle(_private->thread);
	InterlockedDecrement(&_private->running);
}

bool Thread::suspended() const
{
	return (_private->suspended != 0);
}

bool Thread::running() const
{
	return (_private->running != 0);
}

ThreadId Thread::id() const
{
	return _private->threadId;
}