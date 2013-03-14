/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#import <QuartzCore/QuartzCore.h>
#import <et/platform-ios/openglviewcontroller.h>
#import <et/app/applicationnotifier.h>

using namespace et;

@interface etOpenGLViewController()
{
	EAGLContext* _context;
	etOpenGLView* _glView;
	et::RenderContextParameters _params;
	et::ApplicationNotifier _notifier;
}

@end

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

#if defined(__IPHONE_6_0)

- (BOOL)shouldAutorotate
{
	return (_params.supportedInterfaceOrientations > 0);
}

- (NSUInteger)supportedInterfaceOrientations
{
	NSUInteger result = 0;
	
	if (_params.supportedInterfaceOrientations & InterfaceOrientation_Portrait)
		result += UIInterfaceOrientationMaskPortrait;
	if (_params.supportedInterfaceOrientations & InterfaceOrientation_PortraitUpsideDown)
		result += UIInterfaceOrientationMaskPortraitUpsideDown;
	if (_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeLeft)
		result += UIInterfaceOrientationMaskLandscapeLeft;
	if (_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeRight)
		result += UIInterfaceOrientationMaskLandscapeRight;
	
	return result;
}

#endif

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    if ((toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft) &&
        (_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeLeft)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationLandscapeRight) &&
        (_params.supportedInterfaceOrientations & InterfaceOrientation_LandscapeRight)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationPortrait) &&
        (_params.supportedInterfaceOrientations & InterfaceOrientation_Portrait)) return YES;
    
    if ((toInterfaceOrientation == UIInterfaceOrientationPortraitUpsideDown) &&
        (_params.supportedInterfaceOrientations & InterfaceOrientation_PortraitUpsideDown)) return YES;
    
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

- (void)presentModalViewController:(UIViewController *)modalViewController animated:(BOOL)animated
{
	_notifier.notifyDeactivated();
	[super presentModalViewController:modalViewController animated:animated];
}

- (void)dismissModalViewControllerAnimated:(BOOL)animated
{
	[super dismissModalViewControllerAnimated:animated];
	_notifier.notifyActivated();
}

@end