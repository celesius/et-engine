/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <et/rendering/rendercontextparams.h>
#import <et/platform-ios/openglview.h>

@interface etOpenGLViewController : UIViewController
{
	EAGLContext* _context;
	etOpenGLView* _glView;
	et::RenderContextParameters _params;
}

- (id)initWithParameters:(et::RenderContextParameters)params;
- (void)setRenderContext:(et::RenderContext*)rc;

- (void)beginRender;
- (void)endRender;

@end
