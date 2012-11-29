/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <Psapi.h>
#include <et/app/application.h>

using namespace et;

void Application::platform_init()
{
#if (ET_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void Application::platform_finalize()
{
}

int Application::platform_run()
{
	RenderContextParameters params;
	delegate()->setRenderContextParameters(params); 

	_renderContext = new RenderContext(params, this);
	if (_renderContext->valid())
	{
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
	}

	return _exitCode;
}

void Application::enterRunLoop()
{
	MSG msg = { };

	_running = true;
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

	default:
		break;
	}

	MessageBox(0, message.c_str(), title.c_str(), alType);
}

void Application::setTitle(const std::string& s)
{
	SendMessage(reinterpret_cast<HWND>(_renderingContextHandle), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(s.c_str()));
}

size_t Application::memoryUsage() const
{
	PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, pmc.cb);
	return pmc.WorkingSetSize;
}