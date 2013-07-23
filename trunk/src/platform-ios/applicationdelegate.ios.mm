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

#if defined(ET_SUPPORT_GOOGLE_PLUS)
#	include <GooglePlus/GooglePlus.h>
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
	(void)application;
	(void)launchOptions;
	
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
	(void)application;
	_notifier.notifyDeactivated();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	(void)application;
	_notifier.notifyActivated();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	(void)application;
	et::application().quit(0);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	(void)application;
	_notifier.notifySuspended();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	(void)application;
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

- (NSUInteger)application:(UIApplication*)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
	(void)application;
	(void)window;
	UIViewController* vc = reinterpret_cast<UIViewController*>(et::application().renderingContextHandle());
	return [vc supportedInterfaceOrientations];
}

#endif


- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url
	sourceApplication:(NSString*)sourceApplication annotation:(id)annotation
{
	BOOL processed = NO;
	
#if defined(ET_SUPPORT_FACEBOOK_SDK)
	
    processed = [FBAppCall handleOpenURL:url sourceApplication:sourceApplication];
	
#endif
	
#if defined(ET_SUPPORT_GOOGLE_PLUS)
	if (!processed)
	{
		processed =[GPPURLHandler handleURL:url sourceApplication:sourceApplication
			annotation:annotation];
	}
#endif
	
	(void)sourceApplication;
	(void)application;
	(void)annotation;
	(void)url;
	
	return processed;
}


@end
