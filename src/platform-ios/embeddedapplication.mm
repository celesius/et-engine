/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <GLKit/GLKit.h>
#include <et/app/applicationnotifier.h>
#include <et/platform-ios/embeddedapplication.h>

using namespace et;

@interface etApplication()
{
	ApplicationNotifier _notifier;
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
		_loaded = NO;
	}
	return self;
}

- (void)loadedInViewController:(UIViewController*)viewController
{
	NSAssert(_loaded == NO, @"Method [etApplication loadedInViewController:] should be called once.");
	
	RenderState::State state = RenderState::currentState();
	
	GLint defaultFrameBufferId = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFrameBufferId);

	application().run(0, 0);
	
	Framebuffer defaultFrameBuffer =
		[self renderContext]->framebufferFactory().createFramebufferWrapper(defaultFrameBufferId);

	vec2i contextSize(viewController.view.bounds.size.width, viewController.view.bounds.size.height);
	defaultFrameBuffer->forceSize(contextSize);

	_notifier.accessRenderContext()->renderState().setDefaultFramebuffer(defaultFrameBuffer);
	_notifier.accessRenderContext()->renderState().applyState(state);

	_loaded = YES;
}

- (void)unloadedInViewController:(UIViewController*)viewController
{
	NSAssert(_loaded == YES, @"Method [etApplication unloadedInViewController:] should be called once.");
	
	application().quit();
	_loaded = NO;
}

- (void)dealloc
{
	application().quit();
	[super dealloc];
}

- (et::RenderContext*)renderContext
{
	return _notifier.accessRenderContext();
}

- (et::IApplicationDelegate*)applicationDelegate
{
	return application().delegate();
}

@end
