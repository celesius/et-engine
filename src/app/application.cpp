/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/threading/threading.h>
#include <et/app/application.h>

using namespace et;

IApplicationDelegate* et::Application::_delegate = 0;

Application::Application() : _renderContext(0), _exitCode(0), _lastQueuedTimeMSec(queryTimeMSec()),
	_fpsLimitMSec(0), _running(false), _active(false)
{
	threading();
	delegate();
	platform_init();
}

Application::~Application()
{
	platform_finalize();
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
		_parameters.push_back(argv[i]);

	return platform_run();
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

	_lastQueuedTimeMSec = queryTimeMSec();
	
	if (_active)
	{
		_runLoop->update(_lastQueuedTimeMSec);
		_delegate->idle(_runLoop->mainTimerPool()->actualTime());
		performRendering();
	}
	
	uint64_t deltaTime = queryTimeMSec() - _lastQueuedTimeMSec;
	if (_fpsLimitMSec >= deltaTime)
		Thread::sleepMSec(_fpsLimitMSec - deltaTime);
}

void Application::setFrameRateLimit(size_t value)
{
	_fpsLimitMSec = (value == 0) ? 0 : 1000 / value;
}

void Application::setActive(bool active)
{
	if (_active == active) return;

	_active = active;

	if (_active)
	{
		_lastQueuedTimeMSec = queryTimeMSec();
		_runLoop->update(_lastQueuedTimeMSec);
		_runLoop->resume();
		_delegate->applicationWillActivate();
	}
	else
	{
		_runLoop->pause();
		_delegate->applicationWillDeactivate();
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