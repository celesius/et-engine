/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#import <QuartzCore/QuartzCore.h>
#import <et/platform-ios/openglviewcontroller.h>

using namespace et;

@implementation etOpenGLViewController

- (id)initWithParameters:(RenderContextParameters)params
{
	self = [super init];
	
	if (self)
	{
		_params = params;
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		_glView = [[etOpenGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds] parameters:_params];
		self.view = _glView;
		
		if (!_context)
		{
			NSLog(@"Failed to create ES context");
			return 0;
		}
		
		if (![EAGLContext setCurrentContext:_context])
		{
			[_context release];
			NSLog(@"Failed to set ES context current");
			return 0;
		}
	}
	return self;
}

- (void)setRenderContext:(et::RenderContext*)rc
{
	[_glView setRenderContext:rc];
	[_glView setContext:_context];
}

- (void)dealloc
{
    if ([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];
	
	[_context release];
    [super dealloc];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if ([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];
	
	[_context release];
	_context = nil;	
}
				   
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    if ((toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft) &&
        ((_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeLeft) == InterfaceOrientation_LandscapeLeft)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationLandscapeRight) &&
        ((_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeRight) == InterfaceOrientation_LandscapeRight)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationPortrait) &&
        ((_params.supportedInterfaceOrientations & InterfaceOrientation_Portrait) == InterfaceOrientation_Portrait)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationPortraitUpsideDown) &&
        ((_params.supportedInterfaceOrientations & InterfaceOrientation_PortraitUpsideDown) == InterfaceOrientation_PortraitUpsideDown)) return YES;
    
    return NO;
}

- (void)beginRender
{
	[_glView beginRender];
}

- (void)endRender
{
	[_glView endRender];
}

@end
