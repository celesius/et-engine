#import <QuartzCore/QuartzCore.h>
#import <et/platform/ios/openglviewcontroller.h>

using namespace et;

@implementation etOpenGLViewController

- (id)initWithParameters:(RenderContextParameters)params
{
	self = [super init];
	
	if (self)
	{
		_params = params;
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		_glView = [[etOpenGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
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
	return (UIInterfaceOrientationIsLandscape(toInterfaceOrientation) && _params.supportLandscapeOrientation) ||
			((toInterfaceOrientation == UIInterfaceOrientationPortrait) && _params.supportPortraitOrientation);
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
