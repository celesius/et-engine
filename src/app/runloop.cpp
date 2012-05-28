/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>
#include <algorithm>
#include <et/app/runloop.h>
#include <et/tasks/tasks.h>

using namespace et;

RunLoopObject::RunLoopObject() : _mainTimerPool(new TimerPoolObject),
	_actualTime(0.0f), _time(0.0f), _activityTime(0.0f), _started(false), _active(true)
{
	attachTimerPool(_mainTimerPool);
}

RunLoopObject::~RunLoopObject()
{
}

void RunLoopObject::update(float t)
{
	updateTime(t);

	if (_active) 
	{
		_taskPool.update(_time);
		TimerPoolList::iterator i = _timerPools.begin();
		while (i != _timerPools.end())
		{
			(*i)->update(_time);
			++i;
		}
	}
}

void RunLoopObject::addTask(Task* t, float delay)
{
	_taskPool.addTask(t, delay);
}

void RunLoopObject::attachTimerPool(TimerPool& pool)
{
	if (std::find(_timerPools.begin(), _timerPools.end(), pool) == _timerPools.end())
	{
		pool->setOwner(this);
		_timerPools.push_back(pool);
	}
}

void RunLoopObject::detachTimerPool(TimerPool& pool)
{
	TimerPoolList::iterator i = _timerPools.begin();

	while (i != _timerPools.end())
	{
		const TimerPool& p = *i;
		if (p == pool)
		{
			pool->setOwner(0);
			_timerPools.erase(i);
			break;
		}
		else 
		{
			++i;
		}
	}
}

void RunLoopObject::detachAllTimerPools()
{
	for (TimerPoolList::iterator i = _timerPools.begin(), e = _timerPools.end(); i != e; ++i)
		(*i)->setOwner(0);
	_timerPools.clear();
}

void RunLoopObject::pause()
{
	_active = false;
}

void RunLoopObject::resume()
{
	if (_active) return;

	_active = true;
	_activityTime = _actualTime;
}

void RunLoopObject::updateTime(float t)
{
	_actualTime = t;

	if (!_started)
	{
		_started = true;
		_activityTime = _actualTime;
	}

	if (_active) 
	{
		_time += t - _activityTime;
		_activityTime = t;
	}
}
