/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <et/app/application.h>
#include <et/opengl/opengl.h>
#include <et/core/tools.h>
#include <et/threading/threading.h>

#if (ET_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

using namespace et;

IApplicationDelegate* et::Application::_delegate = 0;

Application::Application() : 
	_renderContext(0), _exitCode(0), _lastQueuedTime(queryTime()), _fpsLimit(1.0f / 100.0f),
	_running(false), _active(false), _fpsLimitEnabled(true), _cpuLimit(true)
{
#if (ET_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	threading();
}

Application::~Application()
{
	_runLoop.reset(0);
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

	RenderContextParameters params;
	delegate()->setRenderContextParameters(params); 

	_running = true;
	_renderContext = new RenderContext(params, this);

	if (!_running)
		return _exitCode;

	_active = true;

	_lastQueuedTime = queryTime();
	_runLoop = RunLoop(new RunLoopObject);
	_runLoop->update(_lastQueuedTime);

	_renderingContextHandle = _renderContext->renderingContextHandle();
	_renderContext->init();

	_delegate->applicationDidLoad(_renderContext);
	_delegate->applicationWillResizeContext(_renderContext->sizei());

	_lastQueuedTime = queryTime();
	enterRunLoop(); 

	_delegate->applicationWillTerminate();

	delete _delegate, _delegate = 0;
	delete _renderContext, _renderContext = 0;

	return _exitCode;
}

void Application::enterRunLoop()
{
	MSG msg = { };

	while (_running)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			idle();
		}
	}
}

void Application::quit(int exitCode)
{
	_running = false;
	_exitCode = exitCode;
}

void Application::performRendering()
{
	_renderContext->beginRender();
	_delegate->render(_renderContext);
	_renderContext->endRender();
}

void Application::idle()
{ 
	float t = queryTime();
	if (_fpsLimitEnabled && (t - _lastQueuedTime < _fpsLimit)) return;

	_lastQueuedTime = t;

	_delegate->idle(t);
	_runLoop->update(t);
	performRendering();

	if (_cpuLimit)
		SleepEx(1, true);
}

void Application::alert(const std::string& title, const std::string& message, AlertType type)
{
	UINT alType = MB_ICONINFORMATION;

	switch (type)
	{
	case AlertType_Warning: 
		{
			alType = MB_ICONWARNING;
			break;
		}
	case AlertType_Error: 
		{
			alType = MB_ICONERROR;
			break;
		}
	default: { }
	}

	MessageBox(0, message.c_str(), title.c_str(), alType);
}

void Application::setFPSLimit(size_t value)
{
	_fpsLimitEnabled = value > 0;
	_fpsLimit = (value > 0) ? 1.0f / static_cast<float>(value) : 0.0f;
}

void Application::setActive(bool active)
{
	if (_active == active) return;
	_active = active;

	if (_delegate == 0) return;

	Invocation i;
	i.setTarget(_delegate, _active ? &IApplicationDelegate::applicationWillActivate :
		&IApplicationDelegate::applicationWillDeactivate);
	i.invoke();
}

void Application::contextResized(const vec2i& size)
{
	_delegate->applicationWillResizeContext(size);
	performRendering();
}


void Application::setTitle(const std::string& s)
{
	SendMessage(reinterpret_cast<HWND>(_renderingContextHandle), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(s.c_str()));
}

size_t Application::memoryUsage() const
{
	PROCESS_MEMORY_COUNTERS pmc = { } ;
	pmc.cb = sizeof(pmc);
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, pmc.cb);

	return pmc.WorkingSetSize;
}