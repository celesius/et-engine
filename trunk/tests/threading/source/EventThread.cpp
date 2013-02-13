//
//  EventThread.cpp
//  osx
//
//  Created by Sergey Reznik on 13.02.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#include <et/core/tools.h>
#include <et/app/invocation.h>
#include "EventThread.h"

using namespace demo;
using namespace et;

static AtomicCounter threadCounter;

EventThread::EventThread() :
	Thread(false), _runLoop(new RunLoop)
{
	run();
}

EventThread::~EventThread()
{
}

void EventThread::action()
{
	printf(".");
	fflush(stdout);
}

void EventThread::pushActionToRunLoop(EventThread* thread, et::RunLoop::Pointer rl)
{
	if (rl.valid())
	{
		Invocation i;
		i.setTarget(thread, &EventThread::action);
		i.invokeInRunLoop(rl.reference());
	}
}

et::ThreadResult EventThread::main()
{
	size_t index = threadCounter.retain();
	printf("+ %zd\n", index);
	fflush(stdout);

	RunLoop::Pointer rl = _runLoop;
	while (rl->atomicCounterValue() > 1)
	{
		rl->update(queryTimeMSec());
		sleepMSec(10);
	}

	printf("- %zd (%d)\n", index, threadCounter.release());
	fflush(stdout);
	
	return 0;
}