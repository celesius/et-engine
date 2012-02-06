#pragma once

#include <et/timers/timerpool.h>
#include <et/tasks/taskpool.h>

namespace et
{
	class RunLoopObject : public Shared
	{
	public:
		RunLoopObject();
		
		void update(float t);
		void pause();
		void resume();

		inline TimerPool& mainTimerPool()
			{ return _mainTimerPool; }

		inline float time() const
			{ return _time; }

		void attachTimerPool(TimerPool& pool);
		void detachTimerPool(TimerPool& pool);
		void detachAllTimerPools();

		void addTask(Task* t, float delay = 0.0f);

	private:
		void updateTime(float t);

	private:
		TimerPoolList _timerPools;
		TimerPool _mainTimerPool;
		TaskPool _taskPool;
		float _actualTime;
		float _time;
		float _activityTime;
		bool _started;
		bool _active;
	};

	typedef IntrusivePtr<RunLoopObject> RunLoop;
}