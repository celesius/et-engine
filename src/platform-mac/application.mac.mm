/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <AppKit/AppKit.h>
#include <et/app/applicationnotifier.h>

using namespace et;

/*
 * etApplicationDelegate Interface
 */

@interface etApplicationDelegate : NSObject<NSApplicationDelegate>
{
	ApplicationNotifier _notifier;
	NSTimer* _updateTimer;
}

- (void)run;
- (void)onTick:(id)sender;

@end

/*
 * Application implementation
 */
IApplicationDelegate* et::Application::_delegate = 0;

Application::Application() : _renderContext(0), _exitCode(0), _renderingContextHandle(0), _lastQueuedTime(0.0f),
	_fpsLimit(0), _running(false), _active(false), _fpsLimitEnabled(false), _cpuLimit(false)
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
	@autoreleasepool
	{
		[[NSApplication sharedApplication] setDelegate:[[[etApplicationDelegate alloc] init] autorelease]];
		[[NSApplication sharedApplication] run];
		
		return 0;
	}
}

void Application::loaded()
{
	_lastQueuedTime = queryTime();
	_runLoop = RunLoop(new RunLoopObject);
	_runLoop->update(_lastQueuedTime);
	
	RenderContextParameters parameters;
	delegate()->setRenderContextParameters(parameters);
	
	_renderContext = new RenderContext(parameters, this);
	_renderContext->init();
	
	etApplicationDelegate* appDelegate = [[NSApplication sharedApplication] delegate];
	[appDelegate run];
	
	_active = true;
	enterRunLoop();
}

void Application::enterRunLoop()
{
	_running = true;
	delegate()->applicationDidLoad(_renderContext);
}

void Application::quit(int exitCode)
{
	[[NSApplication sharedApplication] terminate:nil];
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
		usleep(0);
}

void Application::alert(const std::string&, const std::string&, AlertType)
{
	
}

void Application::setFPSLimit(size_t)
{
	
}

void Application::setActive(bool active)
{
	if (active == _active) return;
	
	_active = active;
}

void Application::contextResized(const vec2i& size)
{
	
}

/*
 * etApplicationDelegate implementation
 */

@implementation etApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	_notifier.notifyLoaded();
}

- (void)run
{
	_updateTimer = [NSTimer scheduledTimerWithTimeInterval:1.0f / 60.0f	target:self selector:@selector(onTick:)
												  userInfo:nil repeats:YES];
}

- (void)stop
{
	[_updateTimer invalidate], _updateTimer = nil;
}

- (void)onTick:(id)sender
{
	_notifier.notifyIdle();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

- (void)applicationDidHide:(NSNotification *)notification
{
	[self stop];
	_notifier.notifyDeactivated();
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
	[self run];
	_notifier.notifyActivated();
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[self stop];
	_notifier.notifyDeactivated();
}

@end
