/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/timers/timerpool.h>
#include <et/tasks/taskpool.h>

namespace et
{
	class RunLoop : public Shared
	{
	public:
		typedef IntrusivePtr<RunLoop> Pointer;

	public:
		RunLoop();
		
		void update(uint64_t t);
		void pause();
		void resume();

		TimerPool& mainTimerPool()
			{ return _timerPools.front(); }

		float time() const
			{ return _time; }

		void attachTimerPool(TimerPool pool);
		void detachTimerPool(TimerPool pool);
		void detachAllTimerPools();

		void addTask(Task* t, float delay = 0.0f);

	private:
		void updateTime(uint64_t t);

	private:
		TimerPoolList _timerPools;
		TaskPool _taskPool;
		
		uint64_t _actualTimeMSec;
		uint64_t _activityTimeMSec;
		
		float _time;
		bool _started;
		bool _active;
	};
}
