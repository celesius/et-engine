/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <algorithm>
#include <et/tasks/taskpool.h>

using namespace et;

TaskPool::TaskPool()
{
}

TaskPool::~TaskPool() 
{
	CriticalSectionScope lock(_csModifying);
	for (TaskList::iterator i = _tasks.begin(), e = _tasks.end(); i != e; ++i)
	{
		Task* t = *i;
		delete t;
	}
}

void TaskPool::addTask(Task* t, float delay)
{
	CriticalSectionScope lock(_csModifying);
	TaskList::const_iterator alreadyAdded = std::find(_tasksToAdd.begin(), _tasksToAdd.end(), t);
	if (alreadyAdded == _tasksToAdd.end())
	{
		t->setExecutionTime(_lastTime + delay);
		_tasksToAdd.push_back(t); 
	}
}

void TaskPool::update(float t)
{
	_lastTime = t;

	_csModifying.enter();
	if (_tasksToAdd.size())
	{
		_tasks.insert(_tasks.end(), _tasksToAdd.begin(), _tasksToAdd.end());
		_tasksToAdd.clear();
	}

	TaskList::iterator i = _tasks.begin();
	while (i != _tasks.end())
	{
		Task* task = (*i);
		if (t >= task->executionTime())
		{
			task->execute();
			delete task;
			i = _tasks.erase(i);
		}
		else
		{
			++i;
		}
	}
	
	_csModifying.leave();
}