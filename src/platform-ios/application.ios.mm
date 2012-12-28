/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <mach/mach.h>

#include <et/core/tools.h>
#include <et/app/application.h>
#include <et/opengl/opengl.h>
#include <et/platform-ios/applicationdelegate.h>

#if defined(ET_EMBEDDED_APPLICATION)
#	include <et/opengl/openglcaps.h>
#else
#	include <et/platform-ios/openglviewcontroller.h>
#endif

using namespace et;

void Application::platform_init()
{
	_env.updateDocumentsFolder(_identifier);
}

void Application::platform_finalize()
{
	delete _delegate, _delegate = nullptr;
	delete _renderContext, _delegate = nullptr;
}

int Application::platform_run()
{
#if defined(ET_EMBEDDED_APPLICATION)
	
	loaded();
	return 0;
	
#else

    @autoreleasepool
	{
		std::string appName = _parameters.front();
		StringDataStorage appNameData(appName.size());
		etCopyMemory(appNameData.data(), appName.c_str(), appName.size());
        return UIApplicationMain(1, (char*[]){ appNameData.data() }, nil, NSStringFromClass([etApplicationDelegate class]));
    }
	
#endif	
}

void Application::loaded()
{
	UIWindow* mainWindow = [[[UIApplication sharedApplication] delegate] window];
	float scaleFactor = [[UIScreen mainScreen] scale];
	
	RenderContextParameters params;
	params.contextSize = vec2i(static_cast<int>(mainWindow.frame.size.width * scaleFactor),
							   static_cast<int>(mainWindow.frame.size.height * scaleFactor));
	
	delegate()->setRenderContextParameters(params);
	
	_renderContext = new RenderContext(params, this);
	_renderingContextHandle = _renderContext->renderingContextHandle();
	
	_runLoop = RunLoop(new RunLoopObject);
	_runLoop->update(_lastQueuedTime);
	
	_renderContext->init();
    
#if defined(ET_EMBEDDED_APPLICATION)
    
    enterRunLoop();
    
#else    
	[mainWindow setRootViewController:(etOpenGLViewController*)_renderingContextHandle];
	[mainWindow makeKeyAndVisible];
    
	etApplicationDelegate* d = (etApplicationDelegate*)[[UIApplication sharedApplication] delegate];
	[d beginUpdates];
	
	enterRunLoop();
#endif	
}

void Application::enterRunLoop()
{
#if defined(ET_EMBEDDED_APPLICATION)
	_active = true;
#endif
	
	_running = true;
	delegate()->applicationDidLoad(_renderContext);
}

void Application::quit(int exitCode)
{
	_running = false;
	
#if defined(ET_EMBEDDED_APPLICATION)
	
	_active = false;
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

void Application::alert(const std::string& title, const std::string& message, AlertType)
{
	NSString* nsTitle = [NSString stringWithCString:title.c_str() encoding:NSASCIIStringEncoding];
	NSString* nsMessage = [NSString stringWithCString:message.c_str() encoding:NSASCIIStringEncoding];
	[[[[UIAlertView alloc] initWithTitle:nsTitle message:nsMessage delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil] autorelease] show];
}

size_t Application::memoryUsage() const
{
	struct task_basic_info info = { };
	mach_msg_type_number_t size = sizeof(info);
	kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
	return (kerr == KERN_SUCCESS) ? info.resident_size : 0;
}

void Application::setTitle(const std::string &s)
{
}