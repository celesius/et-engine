/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <AppKit/NSApplication.h>
#include <AppKit/NSMenu.h>
#include <et/app/applicationnotifier.h>

using namespace et;

/*
 * etApplicationDelegate Interface
 */

@interface etApplicationDelegate : NSObject<NSApplicationDelegate>
{
	ApplicationNotifier _notifier;
}

@end

/*
 * Application implementation
 */
void Application::loaded()
{
	_lastQueuedTimeMSec = queryTimeMSec();
	_runLoop.update(_lastQueuedTimeMSec);
	
	RenderContextParameters parameters;
	delegate()->setRenderContextParameters(parameters);
	
	_renderContext = new RenderContext(parameters, this);
	
	NSMenu* mainMenu = [[[NSMenu allocWithZone:[NSMenu menuZone]] init] autorelease];
	NSMenuItem* applicationMenuItem = [[[NSMenuItem allocWithZone:[NSMenu menuZone]] init] autorelease];
	[mainMenu addItem:applicationMenuItem];
	[[NSApplication sharedApplication] setMainMenu:mainMenu];
	
	NSString* quitTitle = [NSString stringWithFormat:@"Quit %@", [[NSProcessInfo processInfo] processName]];
	NSMenuItem* quitItem = [[[NSMenuItem allocWithZone:[NSMenu menuZone]] initWithTitle:quitTitle
		action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
	
	NSMenu* applicationMenu = [[[NSMenu allocWithZone:[NSMenu menuZone]] init] autorelease];
	[applicationMenu addItem:quitItem];
	[applicationMenuItem setSubmenu:applicationMenu];
		
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
	[[NSApplication sharedApplication] terminate:nil];
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
	@autoreleasepool
	{
		[[NSApplication sharedApplication] setDelegate:[[[etApplicationDelegate alloc] init] autorelease]];
		[[NSApplication sharedApplication] run];
		return 0;
	}
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

/*
 *
 * etApplicationDelegate implementation
 *
 */

@implementation etApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	_notifier.notifyLoaded();
}

- (void)applicationWillBecomeActive:(NSNotification *)notification
{
	_notifier.notifyActivated();
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
	_notifier.notifyDeactivated();
}

- (void)applicationDidHide:(NSNotification *)notification
{
	_notifier.notifyDeactivated();
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
	_notifier.notifyActivated();
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	_notifier.notifyDeactivated();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

@end
