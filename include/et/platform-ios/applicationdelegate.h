/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once


#import <et/app/events.h>

namespace et
{
	class ApplicationNotifier;
}

@interface etApplicationDelegate : UIResponder <UIApplicationDelegate> 
{
@private	
	et::ApplicationNotifier* _notifier;
	UIWindow* _window;
	CADisplayLink* _displayLink;
	BOOL _updating;
}

@property (nonatomic, retain) UIWindow* window;

- (void)beginUpdates;
- (BOOL)updating;
- (void)endUpdates;

@end