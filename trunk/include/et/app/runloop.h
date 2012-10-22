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
	class RunLoopObject : public Shared
	{
	public:
		RunLoopObject();
		~RunLoopObject();
		
		void update(float t);
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
		void updateTime(float t);

	private:
		TimerPoolList _timerPools;
		TaskPool _taskPool;
		float _actualTime;
		float _time;
		float _activityTime;
		bool _started;
		bool _active;
	};

	typedef IntrusivePtr<RunLoopObject> RunLoop;
}