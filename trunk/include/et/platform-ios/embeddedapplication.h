/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#import <UIKit/UIKit.h>

namespace et
{
	class RenderContext;
	class IApplicationDelegate;
}

@interface etApplication : NSObject

+ (etApplication*)sharedApplication;

- (void)loadedInViewController:(UIViewController*)viewController;
- (void)unloadedInViewController:(UIViewController*)viewController;

- (et::RenderContext*)renderContext;
- (et::IApplicationDelegate*)applicationDelegate;

@end
