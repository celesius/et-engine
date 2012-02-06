#import <QuartzCore/QuartzCore.h>
#import <et/app/application.h>
#import <et/platform/ios/applicationdelegate.h>
#import <et/platform/ios/openglviewcontroller.h>

namespace et
{
	class ApplicationNotifier
	{
	public:
		void notifyLoaded()
			{ application().loaded(); }
		
		void notifyIdle()
			{ application().idle(); }
		
		void notifyDeactivated()
			{ application().setActive(false); }
		
		void notifyActivated()
			{ application().setActive(true); }
	};
}

using namespace et;

@implementation etApplicationDelegate

@synthesize window = _window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
	
	_updating = NO;
	_notifier = new ApplicationNotifier();
	_notifier->notifyLoaded();
	
    return YES;
}

- (void)dealloc
{
	delete _notifier;
    [_window release];
    [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	_notifier->notifyDeactivated();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	_notifier->notifyActivated();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	et::application().quit(0);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	_notifier->notifyDeactivated();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	_notifier->notifyActivated();
}

- (void)tick
{
	_notifier->notifyIdle();
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

@end
