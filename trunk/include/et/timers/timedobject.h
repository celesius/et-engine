#pragma once

#include <list>
#include <et/tasks/tasks.h>

namespace et
{
	class TimerPoolObject;
	class TimedObjectPrivate;
	class TimedObject 
	{
	public:
		TimedObject();

		virtual bool running() const
			{ return _running; }

		inline bool released() const
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
		TimedObjectPrivate* _private;
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