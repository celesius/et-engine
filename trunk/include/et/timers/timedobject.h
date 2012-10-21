/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <list>
#include <et/tasks/tasks.h>

namespace et
{
	class TimerPoolObject;
	class TimedObject 
	{
	public:
		TimedObject();

		virtual bool running() const
			{ return _running; }

		bool released() const
			{ return _released; }

		virtual void cancelUpdates();
		virtual void destroy();

	protected:
		friend class TimerPoolObject;
		friend class TimedObjectDeletionTask;

		virtual ~TimedObject();
		virtual void startUpdates(TimerPoolObject* timerPool = 0);
		virtual void update(float) {  }

		float actualTime();

	private:
		TimerPoolObject* _owner;
		bool _running;
		bool _released;
	};

	class TimedObjectDeletionTask : public Task
	{
	public:
		TimedObjectDeletionTask(TimedObject* obj) : _object(obj) { }

		void execute()
			{ delete _object; }

	private:
		TimedObject* _object;
	};
}