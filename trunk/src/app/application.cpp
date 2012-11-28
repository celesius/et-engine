/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/threading/threading.h>
#include <et/app/application.h>

using namespace et;

Application::Application() : _renderContext(0), _exitCode(0), _lastQueuedTime(queryTime()), 
	_fpsLimit(0), _running(false), _active(false)
{
	threading();
	platform_init();
}

Application::~Application()
{
}

IApplicationDelegate* Application::delegate()
{
	if (_delegate == 0)
		_delegate = initApplicationDelegate();

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
	_lastQueuedTime = queryTime();
	_runLoop->update(_lastQueuedTime);
	_delegate->idle(_runLoop->mainTimerPool()->actualTime());
	performRendering();

	if (_fpsLimit > 0)
	{
		size_t msec = static_cast<size_t>(1000.0f * (queryTime() - _lastQueuedTime));
		if (_fpsLimit > msec)
			Thread::sleep(_fpsLimit - msec);
	}
}

void Application::setFrameRateLimit(size_t value)
{
	_fpsLimit = (value == 0) ? 0 : (1000 / value);
}

void Application::setActive(bool active)
{
	if (_active == active) return;

	_active = active;

	if (_active)
	{
		_lastQueuedTime = queryTime();
		_runLoop->update(_lastQueuedTime);
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
	_delegate->applicationWillResizeContext(size);
}
