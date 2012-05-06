//
//  etnode.h
//  pictorial2
//
//  Created by Sergey Reznik on 2/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

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
