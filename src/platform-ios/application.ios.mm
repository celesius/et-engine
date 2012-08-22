/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>

#include <et/core/tools.h>
#include <et/app/application.h>
#include <et/opengl/opengl.h>
#include <et/platform-ios/applicationdelegate.h>

#if defined(ET_EMBEDDED_APPLICATION)
#include <et/opengl/openglcaps.h>
#else
#include <et/platform-ios/openglviewcontroller.h>
#endif

using namespace et;

IApplicationDelegate* et::Application::_delegate = 0;

Application::Application() : 
	_renderContext(0), _running(false), _exitCode(0), _lastQueuedTime(queryTime())
{
}

Application::~Application()
{
	delete _delegate;
	delete _renderContext;
}

IApplicationDelegate* Application::delegate()
{
	if (_delegate == 0)
		_delegate = initApplicationDelegate();
    
	return _delegate;
}

int Application::run(int argc, char* argv[])
{
#if defined(ET_EMBEDDED_APPLICATION)
    
	loaded();
	return 0;
    
#else	
    
    @autoreleasepool 
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([etApplicationDelegate class]));
    }
    
#endif	
}

void Application::loaded()
{
	RenderContextParameters params;
	delegate()->setRenderContextParameters(params);
	
	_renderContext = new RenderContext(params, this);
	_renderingContextHandle = _renderContext->renderingContextHandle();
	
	_runLoop = RunLoop(new RunLoopObject);
	_runLoop->update(_lastQueuedTime);
	
	_renderContext->init();
    
#if defined(ET_EMBEDDED_APPLICATION)
    
    enterRunLoop();
    
#else    
	UIWindow* mainWindow = [[[UIApplication sharedApplication] delegate] window];
	[mainWindow setRootViewController:(etOpenGLViewController*)_renderingContextHandle];
	[mainWindow makeKeyAndVisible];
    
	etApplicationDelegate* d = (etApplicationDelegate*)[[UIApplication sharedApplication] delegate];
	[d beginUpdates];
	
	enterRunLoop();
#endif	
}

void Application::enterRunLoop()
{
	_running = true;
	delegate()->applicationDidLoad(_renderContext);
}

void Application::quit(int exitCode)
{
	_running = false;
#if defined(ET_EMBEDDED_APPLICATION)
	_delegate->applicationWillTerminate();
	delete _delegate;
	delete _renderContext;
	
	_delegate = 0;
	_renderContext = 0;
	_runLoop.reset(0);
#else	
	exit(exitCode);
#endif	
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
    
    if (_running && _active)
    {
        _delegate->idle(_lastQueuedTime);
        performRendering();
    }
	
	usleep(0);
}

void Application::alert(const std::string& title, const std::string& message, AlertType)
{
	NSString* nsTitle = [NSString stringWithCString:title.c_str() encoding:NSASCIIStringEncoding];
	NSString* nsMessage = [NSString stringWithCString:message.c_str() encoding:NSASCIIStringEncoding];
	[[[[UIAlertView alloc] initWithTitle:nsTitle message:nsMessage delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil] autorelease] show];
}

void Application::setFPSLimit(size_t)
{
}

void Application::setActive(bool active)
{
	if (_active == active) return;
	
	_active = active;
	
	if (_active)
		_delegate->applicationWillActivate();
	else
		_delegate->applicationWillDeactivate();
}

void Application::contextResized(const vec2i& size)
{
	if (_running)
	{
		_delegate->applicationWillResizeContext(size);
		performRendering();
	}
}