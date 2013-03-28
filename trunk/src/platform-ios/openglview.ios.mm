/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#import <QuartzCore/QuartzCore.h>
#import <et/platform-ios/openglview.h>
#import <et/rendering/rendercontext.h>

extern NSString* etKeyboardRequiredNotification;
extern NSString* etKeyboardNotRequiredNotification;

class et::RenderContextNotifier
{
public:
	void resized(const et::vec2i& sz, et::RenderContext* rc)
		{ rc->resized(sz); }
};

using namespace et;

@interface etOpenGLView()
{
    EAGLContext* _context;
	et::Framebuffer _defaultFramebuffer;
	et::RenderContext* _rc;
	et::RenderContextNotifier* _rcNotifier;
	et::Input::PointerInputSource _pointerInputSource;
	et::Input::KeyboardInputSource _keyboardInputSource;
	BOOL _keyboardAllowed;
}

- (void)performInitializationWithParameters:(const RenderContextParameters&)params;

- (void)createFramebuffer;
- (void)deleteFramebuffer;
- (void)onNotificationRecevied:(NSNotification*)notification;

@end

@implementation etOpenGLView

@synthesize context = _context;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame parameters:(et::RenderContextParameters&)params
{
	self = [super initWithFrame:frame];
	
	if (self)
	{
		[self performInitializationWithParameters:params];
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

- (void)performInitializationWithParameters:(const RenderContextParameters&)params
{
	_rcNotifier = new RenderContextNotifier;
	
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
		kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	_context = nil;
	_keyboardAllowed = NO;
	
	[[NSNotificationCenter defaultCenter] addObserver:self
		selector:@selector(onNotificationRecevied:) name:etKeyboardRequiredNotification object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
		selector:@selector(onNotificationRecevied:) name:etKeyboardNotRequiredNotification object:nil];
	
	self.multipleTouchEnabled = params.multipleTouch;
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
	
	GLenum depthAttachment = GL_DEPTH_ATTACHMENT;
	glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, &depthAttachment);
	
	glBindRenderbuffer(GL_RENDERBUFFER, _defaultFramebuffer->colorRenderbuffer());
	checkOpenGLError("glBindRenderbuffer(GL_RENDERBUFFER, ...)");

	BOOL done = [_context presentRenderbuffer:GL_RENDERBUFFER];
	checkOpenGLError("[_context presentRenderbuffer:GL_RENDERBUFFER]");

	if (!done)
		log::error("presentRenderbuffer failed");
}

- (void)createFramebuffer
{
	self.contentScaleFactor = [[UIScreen mainScreen] scale];
	
	CAEAGLLayer* glLayer = (CAEAGLLayer*)self.layer;
	glLayer.contentsScale = self.contentScaleFactor;
	CGSize layerSize = glLayer.bounds.size;
	
	if (_defaultFramebuffer.invalid())
	{
		vec2i size(layerSize.width, layerSize.height);
		
		_defaultFramebuffer =
			_rc->framebufferFactory().createFramebuffer(size, "DefaultFramebuffer",  0, 0, 0, 0, 0, 0, true);
	}
	
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
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
			_defaultFramebuffer->colorRenderbuffer());
		checkOpenGLError("glFramebufferRenderbuffer(...");
	}

	vec2i size;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &size.x);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &size.y);
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
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size.x, size.y);
	checkOpenGLError("glRenderbufferStorage -> depth");
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
		GL_RENDERBUFFER, _defaultFramebuffer->depthRenderbuffer());

	_defaultFramebuffer->forceSize(size);
	_rcNotifier->resized(size, _rc);
	
	_defaultFramebuffer->checkStatus();
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
		pt.scroll = vec2(0.0f);
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_pointerInputSource.pointerPressed(pt);
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
		pt.scroll = vec2(0.0f);
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_pointerInputSource.pointerMoved(pt);
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
		pt.scroll = vec2(0.0f);
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_pointerInputSource.pointerReleased(pt);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
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
		pt.scroll = vec2(0.0f);
		pt.timestamp = touch.timestamp;
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		_pointerInputSource.pointerCancelled(pt);
	}
}

- (void)onNotificationRecevied:(NSNotification*)notification
{
	if ([notification.name isEqualToString:etKeyboardRequiredNotification])
	{
		_keyboardAllowed = YES;
		[self becomeFirstResponder];
	}
	else if ([notification.name isEqualToString:etKeyboardNotRequiredNotification])
	{
		[self resignFirstResponder];
		_keyboardAllowed = NO;
	}
}

- (BOOL)canBecomeFirstResponder
{
	return _keyboardAllowed;
}

- (BOOL)hasText
{
	return YES;
}

- (void)insertText:(NSString *)text
{
	size_t charValue = 0;
	
	if ([text canBeConvertedToEncoding:NSUTF32LittleEndianStringEncoding])
	{
		NSUInteger actualLength = 0;
		
		[text getBytes:nil maxLength:0 usedLength:&actualLength
		   encoding:NSUTF32LittleEndianStringEncoding options:0
				 range:NSMakeRange(0, [text length]) remainingRange:0];
		
		DataStorage<wchar_t> result(actualLength + 1);
		
		[text getBytes:result.data() maxLength:result.dataSize() usedLength:0
		   encoding:NSUTF32LittleEndianStringEncoding options:0
				 range:NSMakeRange(0, [text length]) remainingRange:0];
		
		charValue = result[0];
	}
	else
	{
		NSLog(@"Unable to convert %@ to NSUTF32LittleEndianStringEncoding", text);
	}
	
	_keyboardInputSource.charEntered(charValue);
}

- (void)deleteBackward
{
	_keyboardInputSource.charEntered(ET_BACKSPACE);
}

@end
