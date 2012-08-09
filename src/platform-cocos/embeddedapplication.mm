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
	
	RenderState::State state = RenderState::currentState();
	
	application().run(0, 0);
	_notifier->accessRenderContext()->renderState().setDefaultFramebuffer(
		[self renderContext]->framebufferFactory().createFramebufferWrapper([[view valueForKey:@"renderer_"] defaultFrameBuffer]));
	
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

- (et::RenderContext*)renderContext
{
	return _notifier->accessRenderContext();
}

- (et::IApplicationDelegate*)applicationDelegate
{
	return application().delegate();
}

@end
