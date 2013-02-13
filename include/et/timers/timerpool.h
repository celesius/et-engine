/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/threading/criticalsection.h>
#include <et/timers/timedobject.h>

namespace et
{
	class RunLoop;
	class TimerPoolObjectPrivate;
	class TimerPoolObject : public Shared
	{
	public:
		TimerPoolObject(RunLoop* owner);

		void update(float t);
		float actualTime() const;

		void retain();

		void attachTimedObject(TimedObject* obj);
		void detachTimedObject(TimedObject* obj);
		void deleteTimedObjecct(TimedObject* obj);

		void setOwner(RunLoop* owner)
			{ _owner = owner; }

	private:
		enum QueueAction
		{
			QueueAction_Add,
			QueueAction_Update,
			QueueAction_Remove
		};

		struct QueueEntry
		{
			TimedObject* object;
			QueueAction action;
			QueueEntry(TimedObject* o, QueueAction a) : object(o), action(a) { }

			bool operator == (const QueueEntry& e)
				{ return (e.object == object) && (e.action == action); }
		};

		typedef std::list<QueueEntry> TimerPoolQueue;

	private:
		TimerPoolQueue _timedObjects;
		TimerPoolQueue _queue;
		CriticalSection _lock;
		RunLoop* _owner;

		bool _initialized;
		bool _updating;
	};

	typedef IntrusivePtr<TimerPoolObject> TimerPool;
	typedef std::list<TimerPool> TimerPoolList;
}