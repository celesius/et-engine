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

void Application::setTitle(const std::string &s)
{
}

void Application::alert(const std::string&, const std::string&, AlertType)
{	
}

void Application::platform_init()
{
	_env.updateDocumentsFolder(_identifier);
}

int Application::platform_run()
{
	@autoreleasepool
	{
		[[NSApplication sharedApplication] setDelegate:[[[etApplicationDelegate alloc] init] autorelease]];
		[[NSApplication sharedApplication] run];
		return 0;
	}
}

void Application::platform_finalize()
{
	delete _delegate, _delegate = nullptr;
	delete _renderContext, _renderContext = nullptr;
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
	_notifier.notifyDeactivated();
}

- (void)onTick:(id)sender
{
	_notifier.notifyIdle();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	[self stop];
	return YES;
}

- (void)applicationDidHide:(NSNotification *)notification
{
	[self stop];
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
	[self run];
	_notifier.notifyActivated();
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[self stop];
}

@end
