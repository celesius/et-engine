#include <assert.h>
#include <et/app/application.h>
#include <et/timers/timerpool.h>
#include <et/timers/timedobject.h>

using namespace et;

class et::TimedObjectPrivate
{
public:
	TimerPool timerPool;
};

TimedObject::TimedObject() : _private(new TimedObjectPrivate), _running(false), _released(false)
{
}

TimedObject::~TimedObject()
{
	if (_private->timerPool.valid())
	{
		_private->timerPool->detachTimedObject(this);
		_private->timerPool.reset(0);
	}

	delete _private;
}

void TimedObject::startUpdates(TimerPoolObject* timerPool)
{
	if (_released || _running) return;

	if (_running && _private->timerPool.valid())
		_private->timerPool->detachTimedObject(this);

	_running = true;

	_private->timerPool = timerPool ? TimerPool(timerPool) : mainRunLoop()->mainTimerPool();
	_private->timerPool->attachTimedObject(this); 
}

void TimedObject::cancelUpdates()
{
	_running = false;
}

void TimedObject::destroy()
{
	if (_released) return;

	_running = false;
	_released = true;

	if (_private->timerPool.valid())
		_private->timerPool->deleteTimedObjecct(this);
	else
		mainRunLoop()->addTask(new TimedObjectDeletionTask(this));
}

float TimedObject::actualTime()
{
	if (_private->timerPool.valid())
	{
		return _private->timerPool->actualTime();
	}
	else
	{
		assert("TimedObject isn't attached to timer pool" && 0);
		return 0.0f;
	}
}