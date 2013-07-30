/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/threading/threading.h>
#include <et/app/application.h>

using namespace et;

IApplicationDelegate* et::Application::_delegate = 0;

Application::Application() : _renderContext(0), _exitCode(0), _lastQueuedTimeMSec(queryTimeMSec()),
	_fpsLimitMSec(0), _fpsLimitMSecFractPart(0), _running(false), _active(false), _suspended(false)
{
	threading();
	
	delegate()->setApplicationParameters(_parameters);

	platformInit();
	platformActivate();
}

Application::~Application()
{
	_running = false;
	
	delegate()->applicationWillTerminate();

	platformDeactivate();
	platformFinalize();
}

IApplicationDelegate* Application::delegate()
{
	if (_delegate == nullptr)
	{
		_delegate = initApplicationDelegate();
		assert(_delegate);
		_identifier = _delegate->applicationIdentifier();
	}
    
	return _delegate;
}

int Application::run(int argc, char* argv[])
{
	for (int i = 0; i < argc; ++i)
		_launchParameters.push_back(argv[i]);

	return platformRun();
}

void Application::performRendering()
{
	_renderContext->beginRender();
	_delegate->render(_renderContext);
	_renderContext->endRender();
}

void Application::idle()
{
	assert(_running);

	uint64_t currentTime = queryTimeMSec();
	uint64_t elapsedTime = currentTime - _lastQueuedTimeMSec;

	if (elapsedTime >= _fpsLimitMSec)
	{
		if (!_suspended)
		{
			_runLoop.update(currentTime);
			_delegate->idle(_runLoop.mainTimerPool()->actualTime());
			performRendering();
		}
		
		_lastQueuedTimeMSec = queryTimeMSec();
	}
	else 
	{
		uint64_t sleepInterval = (_fpsLimitMSec - elapsedTime) +
			(rand() % 1000 > _fpsLimitMSecFractPart ? 0 : -1);
		
		Thread::sleepMSec(sleepInterval);
	}
}

void Application::setFrameRateLimit(size_t value)
{
	_fpsLimitMSec = (value == 0) ? 0 : 1000 / value;
	_fpsLimitMSecFractPart = (value == 0) ? 0 : (1000000 / value - 1000 * _fpsLimitMSec);
}

void Application::setActive(bool active)
{
	if (!_running || (_active == active)) return;

	_active = active;

	if (_active)
	{
		if (_suspended)
			resume();

		_delegate->applicationWillActivate();
		platformActivate();
	}
	else
	{
		_delegate->applicationWillDeactivate();
		platformDeactivate();
		
		if (_parameters.shouldSuspendOnDeactivate)
			suspend();
	}
}

void Application::contextResized(const vec2i& size)
{
	if (_running && _active)
	{
		_delegate->applicationWillResizeContext(size);
		performRendering();
	}
}

float Application::cpuLoad() const
{
	return Threading::cpuUsage();
}

void Application::suspend()
{
	if (_suspended) return;

	delegate()->applicationWillSuspend();
	_runLoop.pause();

	platformSuspend();
	
	_suspended = true;
}

void Application::resume()
{
	assert(_suspended && "Should be suspended.");

	delegate()->applicationWillResume();
	
	_suspended = false;

	platformResume();

	_lastQueuedTimeMSec = queryTimeMSec();
	_runLoop.update(_lastQueuedTimeMSec);
	_runLoop.resume();
}
