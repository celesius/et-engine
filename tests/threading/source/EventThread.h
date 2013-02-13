//
//  EventThread.h
//  osx
//
//  Created by Sergey Reznik on 13.02.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#pragma once

#include <et/threading/threading.h>
#include <et/app/runloop.h>

namespace demo
{
	class EventThread : public et::Thread
	{
	public:
		EventThread();
		~EventThread();
		
		et::RunLoop::Pointer runLoop()
			{ return _runLoop; }

		et::ThreadResult main();

		void action();

		void pushActionToRunLoop(EventThread*, et::RunLoop::Pointer rl);

	private:
		et::RunLoop::Pointer _runLoop;
	};
}