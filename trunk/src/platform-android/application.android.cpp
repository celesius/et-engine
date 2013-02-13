/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>

using namespace et;

void Application::loaded()
{
	_lastQueuedTimeMSec = queryTimeMSec();
	_runLoop = RunLoop(new RunLoopObject);
	_runLoop->update(_lastQueuedTimeMSec);
	
	RenderContextParameters parameters;
	delegate()->setRenderContextParameters(parameters);
	
	_renderContext = new RenderContext(parameters, this);
			
	enterRunLoop();
}

void Application::enterRunLoop()
{
	_active = true;
	_running = true;
	
	delegate()->applicationDidLoad(_renderContext);
	
	_renderContext->init();
}

void Application::quit(int exitCode)
{
}

void Application::setTitle(const std::string &s)
{
}

void Application::alert(const std::string&, const std::string&, AlertType)
{	
}

void Application::platformInit()
{
	_env.updateDocumentsFolder(_identifier);
}

int Application::platformRun()
{
	return 0;
}

void Application::platformFinalize()
{
	delete _delegate, _delegate = nullptr;
	delete _renderContext, _renderContext = nullptr;
}

void Application::platformActivate()
{
}

void Application::platformDeactivate()
{
}
