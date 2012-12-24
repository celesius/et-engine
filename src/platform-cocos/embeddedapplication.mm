/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <cocos2d.h>

#include <et/app/applicationnotifier.h>
#include <et/platform-ios/embeddedapplication.h>

using namespace et;

@interface etApplication()
{
	ApplicationNotifier* _notifier;
	BOOL _loaded;
}

@end

@implementation etApplication

static etApplication* _sharedInstance = nil;

/*
 * Singletone Methods
 */
+ (etApplication*)sharedApplication
{
	if (_sharedInstance == nil)
		_sharedInstance = [[etApplication alloc] init];
	
	return _sharedInstance;
}

- (oneway void)release { }
- (NSUInteger)retainCount { return LONG_MAX; }
- (id)retain { return self; }
- (id)autorelease { return self; }

- (id)init
{
	self = [super init];
	if (self)
	{
		_notifier = new ApplicationNotifier();
		_loaded = NO;
	}
	return self;
}

- (void)loadedInViewController:(UIViewController*)vc
{
	NSAssert(_loaded == NO, @"Method [etApplication loaded] should be called once.");
	
	CCGLView* view = (CCGLView*)[[CCDirector sharedDirector] view];
	NSAssert(view, @"Cocos OpenGL view should be initialized before running embedded application.");
	
	[view addObserver:self forKeyPath:@"frame" options:NSKeyValueObservingOptionNew context:nil];
	
	RenderState::State state = RenderState::currentState();
	int defaultFramebuffer = [[view valueForKey:@"renderer_"] defaultFrameBuffer];
	
	application().run(0, 0);
	_notifier->accessRenderContext()->renderState().setDefaultFramebuffer(
		[self renderContext]->framebufferFactory().createFramebufferWrapper(defaultFramebuffer));
	
	_notifier->accessRenderContext()->renderState().applyState(state);
	_loaded = YES;
}

- (void)unloadedInViewController:(UIViewController*)viewController
{
	application().quit();
	delete _notifier, _notifier = 0;
    _loaded = NO;
}

- (void)dealloc
{
	application().quit();
	delete _notifier;
	[super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object
	change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"frame"])
	{
		CGRect frame = { };
		NSValue* value = [change objectForKey:@"new"];
		[value getValue:&frame];
		
		float scaleFactor = [[UIScreen mainScreen] scale];
		vec2i size = vec2i(static_cast<int>(scaleFactor * frame.size.width), static_cast<int>(scaleFactor * frame.size.height));
		RenderContext* rc = _notifier->accessRenderContext();
		if (rc->sizei() != size)
		{
			rc->renderState().defaultFramebuffer()->forceSize(size.x, size.y);
			_notifier->notifyResize(size);
		}
	}
}


- (et::RenderContext*)renderContext
{
	return _notifier->accessRenderContext();
}

- (et::IApplicationDelegate*)applicationDelegate
{
	return application().delegate();
}

@end
