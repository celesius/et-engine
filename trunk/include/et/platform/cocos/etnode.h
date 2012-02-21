//
//  etnode.h
//  pictorial2
//
//  Created by Sergey Reznik on 2/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#import <cocos2d.h>
#import <et/device/rendercontext.h>

@interface etNode : CCNode

- (et::RenderContext*)renderContext;

@end