//
//  etnode.mm
//  pictorial2
//
//  Created by Sergey Reznik on 2/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <ccGLState.h>
#include <et/platform/cocos/etnode.h>
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
		_notifier = new ApplicationNotifier();
	}
	return self;
}

- (void)dealloc
{
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

- (et::IApplicationDelegate*)applicationDelegate
{
	return application().delegate();
}

@end
