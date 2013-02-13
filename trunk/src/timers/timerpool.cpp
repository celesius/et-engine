/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <algorithm>
#include <et/app/application.h>
#include <et/timers/timerpool.h>
#include <et/timers/timedobject.h>

using namespace et;

TimerPoolObject::TimerPoolObject(RunLoop* owner) : _owner(owner)
{
}

void TimerPoolObject::attachTimedObject(TimedObject* obj)
{
	CriticalSectionScope lock(_lock);

	QueueEntry entry(obj, QueueAction_Update);
	if (std::find(_timedObjects.begin(), _timedObjects.end(), entry) != _timedObjects.end()) return;

	if (_updating)
	{
		entry.action = QueueAction_Add;
		if (std::find(_queue.begin(), _queue.end(), entry) == _queue.end())
			_queue.push_back(entry);
	}
	else
	{
		_timedObjects.push_back(entry);
	}
}

void TimerPoolObject::detachTimedObject(TimedObject* obj)
{
	CriticalSectionScope lock(_lock);

	QueueEntry existsAddingEntry(obj, QueueAction_Add);
	TimerPoolQueue::iterator existsAddingValue = std::find(_queue.begin(), _queue.end(), existsAddingEntry);
	if (existsAddingValue != _queue.end())
	{
		if (_updating)
			existsAddingEntry.action = QueueAction_Remove;
		else
			_queue.erase(existsAddingValue);
	}

	QueueEntry existsUpdatingEntry(obj, QueueAction_Update);
	TimerPoolQueue::iterator existsUpdatingValue = std::find(_timedObjects.begin(), _timedObjects.end(), existsUpdatingEntry);
	if (existsUpdatingValue != _timedObjects.end()) 
	{
		if (_updating)
			existsUpdatingValue->action = QueueAction_Remove;
		else
			_timedObjects.erase(existsUpdatingValue);
	}
}

void TimerPoolObject::update(float t)
{
	CriticalSectionScope lock(_lock);

	if (_queue.size())
	{
		for (TimerPoolQueue::iterator i = _queue.begin(), e = _queue.end(); i != e; ++i)
		{
			if (i->action == QueueAction_Add)
			{
				i->action = QueueAction_Update;
				_timedObjects.push_back(*i);
			}
		}

		_queue.clear();
	}

	_updating = true;

	TimerPoolQueue::iterator i = _timedObjects.begin();
	while (i != _timedObjects.end())
	{
		const QueueEntry& obj = *i;

		if (obj.action == QueueAction_Update)
		{
			if (obj.object->running())
				obj.object->update(t);

			i = obj.object->running() ? ++i : _timedObjects.erase(i);
		}
		else if (obj.action == QueueAction_Remove)
		{
			i = _timedObjects.erase(i);
		}
		else
		{
			assert(0 && "Unresolved state for TimerPool QueueEntry");
		}
	} 

	_updating = false;
}

void TimerPoolObject::deleteTimedObjecct(TimedObject* obj)
{
	_owner->addTask(new TimedObjectDeletionTask(obj));
}

float TimerPoolObject::actualTime() const
{
	return _owner->time();
}

void TimerPoolObject::retain()
{
	Shared::retain();
}

