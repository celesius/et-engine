/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
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
		ET_DECLARE_POINTER(RunLoop)
		
	public:
		RunLoop();
		
		void update(uint64_t t);
		void pause();
		void resume();

		TimerPool::Pointer mainTimerPool()
			{ return _timerPools.front(); }

		float time() const
			{ return _time; }

		uint64_t timeMSec() const
			{ return _actualTimeMSec; }
		
		void attachTimerPool(TimerPool::Pointer pool);
		void detachTimerPool(TimerPool::Pointer pool);
		void detachAllTimerPools();

		void addTask(Task* t, float delay = 0.0f);

	private:
		void updateTime(uint64_t t);

	private:
		std::vector<TimerPool::Pointer> _timerPools;
		TaskPool _taskPool;
		
		uint64_t _actualTimeMSec;
		uint64_t _activityTimeMSec;
		
		float _time;
		bool _started;
		bool _active;
	};
}
