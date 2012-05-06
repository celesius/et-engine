/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

namespace et
{
	class RenderContext;
	class IApplicationDelegate;
}

@interface etApplication : NSObject

+ (etApplication*)sharedApplication;

- (void)loaded;
- (et::RenderContext*)renderContext;
- (et::IApplicationDelegate*)applicationDelegate;

@end
