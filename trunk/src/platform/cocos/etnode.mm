//
//  etnode.mm
//  pictorial2
//
//  Created by Sergey Reznik on 2/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <ccGLState.h>
#include <et/platform/cocos/etnode.h>

#include <et/app/application.h>
#include <et/app/applicationnotifier.h>

using namespace et;

@interface etNode()
{
	ApplicationNotifier* _notifier;
}

@end

@implementation etNode

- (id)init
{
	self = [super init];
	if (self)
	{
		CCGLView* view = (CCGLView*)[[CCDirector sharedDirector] view];
		id<CCESRenderer> renderer = [view valueForKey:@"renderer_"];
		GLuint value = [renderer defaultFrameBuffer];
		
		RenderState::State state = RenderState::currentState();
		
		application().run(0, 0);

		Framebuffer defaultFramebuffer = [self renderContext]->framebufferFactory().createFramebufferWrapper(value);
		_notifier->accessRenderContext()->renderState().setDefaultFramebuffer(defaultFramebuffer);

		_notifier->accessRenderContext()->renderState().applyState(state);
		_notifier = new ApplicationNotifier();
		_notifier->accessRenderContext()->renderState().applyState(state);
	}
	return self;
}

- (void)dealloc
{
	application().quit();
	delete _notifier;
	[super dealloc];
}

- (void)onEnter
{
	[super onEnter];
	_notifier->notifyActivated();
}

- (void)onExit
{
	[super onExit];
	_notifier->notifyDeactivated();
}

- (void)draw
{
	RenderState::State state = RenderState::currentState();
	
	_notifier->accessRenderContext()->renderState().applyState(state);
	_notifier->notifyIdle();
	_notifier->accessRenderContext()->renderState().applyState(state);
}

- (et::RenderContext*)renderContext
{
	return _notifier->accessRenderContext();
}

@end
