#pragma once

#include <et/tasks/tasks.h>
#include <et/threading/criticalsection.h>

namespace et
{
	class TaskPool
	{
	public:
		TaskPool();
		~TaskPool();
		void update(float t);

		void addTask(Task* t, float delay = 0.0f);

	private:
		CriticalSection _csModifying;
		TaskList _tasks;
		TaskList _tasksToAdd;
		float _lastTime;
	};


}