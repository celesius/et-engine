#import <QuartzCore/QuartzCore.h>
#import <et/platform/ios/openglview.h>
#import <et/device/rendercontext.h>

class et::RenderContextNotifier
{
public:
	inline void resized(const et::vec2i& sz, et::RenderContext* rc)
		{ rc->resized(sz); }
};

using namespace et;

@interface etOpenGLView(PrivateMethods)

- (void)createFramebuffer;
- (void)deleteFramebuffer;

@end

@implementation etOpenGLView

@synthesize context = _context;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	
	if (self)
	{
		_rcNotifier = new RenderContextNotifier;
		
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		_context = nil;
		self.multipleTouchEnabled = YES;
	}
	
	return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];    
    [_context release];
	delete _rcNotifier;
    [super dealloc];
}

- (void)setRenderContext:(RenderContext*)rc
{
	_rc = rc;
}

- (void)setContext:(EAGLContext*)newContext
{
    if (_context == newContext) return;
	
	[_context release];
	_context = [newContext retain];
	[EAGLContext setCurrentContext:_context];
	[self createFramebuffer];
}

- (void)beginRender
{
	_rc->renderState().bindDefaultFramebuffer();
}

- (void)endRender
{
	checkOpenGLError("endRender");
	
	_rc->renderState().bindDefaultFramebuffer();
	
	glBindRenderbuffer(GL_RENDERBUFFER, _defaultFramebuffer->colorRenderbuffer());
	checkOpenGLError("glBindRenderbuffer(GL_RENDERBUFFER, " + intToStr(_defaultFramebuffer->colorRenderbuffer())+ ")");
	
	BOOL done = [_context presentRenderbuffer:GL_RENDERBUFFER];
	checkOpenGLError("[_context presentRenderbuffer:GL_RENDERBUFFER]");
	
	if (!done)
		std::cout << "presentRenderbuffer failed" << std::endl;
}

- (void)createFramebuffer
{
	self.contentScaleFactor = [[UIScreen mainScreen] scale];
	
	CAEAGLLayer* glLayer = (CAEAGLLayer*)self.layer;
	glLayer.contentsScale = self.contentScaleFactor;
	CGSize layerSize = glLayer.bounds.size;
	
	if (!_defaultFramebuffer.valid())
		_defaultFramebuffer = _rc->framebufferFactory().createFramebuffer(vec2i(layerSize.width, layerSize.height), "DefaultFramebuffer",  0, 0, 0, 0, 0, 0, true);
	
	_rc->renderState().setDefaultFramebuffer(_defaultFramebuffer);
	_rc->renderState().bindDefaultFramebuffer();
	
	if (_defaultFramebuffer->colorRenderbuffer() == 0)
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		checkOpenGLError("glGenRenderbuffers -> color");
		_defaultFramebuffer->setColorRenderbuffer(buf);
	}
	
	glBindRenderbuffer(GL_RENDERBUFFER, _defaultFramebuffer->colorRenderbuffer());
	checkOpenGLError("glBindRenderbuffer -> color");
	
	if ([_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:glLayer])
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _defaultFramebuffer->colorRenderbuffer());
		checkOpenGLError("glFramebufferRenderbuffer(...");
	}
	
	int w = 0;
	int h = 0;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);
	checkOpenGLError("glGetRenderbufferParameteriv");
	
	if (_defaultFramebuffer->depthRenderbuffer() == 0)
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		checkOpenGLError("glGenRenderbuffers -> depth");
		_defaultFramebuffer->setDepthRenderbuffer(buf);
	}	
	glBindRenderbuffer(GL_RENDERBUFFER, _defaultFramebuffer->depthRenderbuffer());
	checkOpenGLError("glBindRenderbuffer -> depth");
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	checkOpenGLError("glRenderbufferStorage -> depth");
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
							  GL_RENDERBUFFER, _defaultFramebuffer->depthRenderbuffer());
	
	_defaultFramebuffer->forceSize(w, h);
	_rcNotifier->resized(vec2i(w, h), _rc);
	
	_defaultFramebuffer->check();
}

- (void)deleteFramebuffer
{
	_defaultFramebuffer = Framebuffer();
}

- (void)layoutSubviews
{
	[self createFramebuffer];
}

- (const Framebuffer&)defaultFramebuffer
{
	return _defaultFramebuffer;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	(void)event;
	float scale = self.contentScaleFactor;
	CGSize ownSize = self.bounds.size;
	ownSize.width *= scale;
	ownSize.height *= scale;
	
	for (UITouch* touch in touches)
	{
		CGPoint touchPoint = [touch locationInView:self];
		touchPoint.x *= scale;
		touchPoint.y *= scale;
		
		PointerInputInfo pt;
		pt.id = [touch hash];
		pt.pos = vec2(touchPoint.x, touchPoint.y);
		pt.scroll = 0;
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_inputSource.pointerPressed(pt);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent*)event
{
	(void)event;
	float scale = self.contentScaleFactor;
	CGSize ownSize = self.bounds.size;
	ownSize.width *= scale;
	ownSize.height *= scale;
	
	for (UITouch* touch in touches)
	{
		CGPoint touchPoint = [touch locationInView:self];
		touchPoint.x *= scale;
		touchPoint.y *= scale;

		PointerInputInfo pt;
		pt.id = [touch hash];
		pt.pos = vec2(touchPoint.x, touchPoint.y);
		pt.scroll = 0;
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_inputSource.pointerMoved(pt);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	(void)event;
	float scale = self.contentScaleFactor;
	CGSize ownSize = self.bounds.size;
	ownSize.width *= scale;
	ownSize.height *= scale;
	
	for (UITouch* touch in touches)
	{
		CGPoint touchPoint = [touch locationInView:self];
		touchPoint.x *= scale;
		touchPoint.y *= scale;
		
		PointerInputInfo pt;
		pt.id = [touch hash];
		pt.pos = vec2(touchPoint.x, touchPoint.y);
		pt.scroll = 0;
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_inputSource.pointerReleased(pt);
	}
}

@end
