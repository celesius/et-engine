//
//  etnode.mm
//  pictorial2
//
//  Created by Sergey Reznik on 2/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <cocos2d.h>
#include <et/app/applicationnotifier.h>
#include <et/platform/cocos/etapplication.cocos.h>

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

- (void)loaded
{
	NSAssert(_loaded == NO, @"Method [etApplication loaded] should be called once.");
	
	_loaded = YES;
	RenderState::State state = RenderState::currentState();
	CCGLView* view = (CCGLView*)[[CCDirector sharedDirector] view];
	NSAssert(view, @"Cocos OpenGL view should be initialized before running embedded application.");
	
	GLuint defaultFramebufferId = [[view valueForKey:@"renderer_"] defaultFrameBuffer];
	
	application().run(0, 0);
	_notifier->accessRenderContext()->renderState().applyState(state);
	
	Framebuffer defaultFramebuffer = [self renderContext]->framebufferFactory().createFramebufferWrapper(defaultFramebufferId);
	_notifier->accessRenderContext()->renderState().setDefaultFramebuffer(defaultFramebuffer);
	
	_notifier->accessRenderContext()->renderState().applyState(state);
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
