/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <QuartzCore/QuartzCore.h>

#if defined(ET_SUPPORT_FACEBOOK_SDK)
#	include <FacebookSDK/FacebookSDK.h>
#endif

#include <et/app/application.h>
#include <et/app/applicationnotifier.h>
#include <et/platform-ios/applicationdelegate.h>
#include <et/platform-ios/openglviewcontroller.h>

using namespace et;

@interface etApplicationDelegate()
{
	et::ApplicationNotifier _notifier;
	UIWindow* _window;
	CADisplayLink* _displayLink;
	BOOL _updating;
}

@end

@implementation etApplicationDelegate

@synthesize window = _window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
	
	_updating = NO;
	_notifier.notifyLoaded();
	
    return YES;
}

- (void)dealloc
{
    [_window release];
    [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	_notifier.notifyDeactivated();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	_notifier.notifyActivated();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	et::application().quit(0);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	_notifier.notifySuspended();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	_notifier.notifyResumed();
}

- (void)tick
{
	_notifier.notifyIdle();
}

- (void)beginUpdates
{
	if (_updating) return;
	
	_displayLink = [[CADisplayLink displayLinkWithTarget:self selector:@selector(tick)] retain];
	[_displayLink setFrameInterval:1];
	[_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (BOOL)updating
{
	return _updating;
}

- (void)endUpdates
{
	if (!_updating) return;
	
	[_displayLink invalidate];
	_displayLink = nil;	
}

#if defined(__IPHONE_6_0)

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
	UIViewController* vc = reinterpret_cast<UIViewController*>(et::application().renderingContextHandle());
	return [vc supportedInterfaceOrientations];
}

#endif

#if defined(ET_SUPPORT_FACEBOOK_SDK)
- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url
	sourceApplication:(NSString*)sourceApplication annotation:(id)annotation
{
    return [FBAppCall handleOpenURL:url sourceApplication:sourceApplication];
}
#endif

@end
