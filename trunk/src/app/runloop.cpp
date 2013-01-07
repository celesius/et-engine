/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <algorithm>
#include <et/app/runloop.h>
#include <et/tasks/tasks.h>

using namespace et;

RunLoopObject::RunLoopObject() :
	_actualTimeMSec(0), _time(0.0f), _activityTimeMSec(0), _started(false), _active(true)
{
	attachTimerPool(TimerPool(new TimerPoolObject(this)));
}

RunLoopObject::~RunLoopObject()
{
	TimerPoolObject* tp = mainTimerPool().ptr();
	_timerPools.clear();
	tp->referenceCount();
}

void RunLoopObject::update(uint64_t t)
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

void RunLoopObject::attachTimerPool(TimerPool pool)
{
	if (std::find(_timerPools.begin(), _timerPools.end(), pool) == _timerPools.end())
	{
		pool->setOwner(this);
		_timerPools.push_back(pool);
	}
}

void RunLoopObject::detachTimerPool(TimerPool pool)
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
	for (auto& i : _timerPools)
		i->setOwner(0);
	
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
	_activityTimeMSec = _actualTimeMSec;
}

void RunLoopObject::updateTime(uint64_t t)
{
	_actualTimeMSec = t;
	
	if (!_started)
	{
		_started = true;
		_activityTimeMSec = _actualTimeMSec;
	}

	if (_active) 
	{
		_time += static_cast<float>(t - _activityTimeMSec) / 1000.0f;
		_activityTimeMSec = t;
	}
}
