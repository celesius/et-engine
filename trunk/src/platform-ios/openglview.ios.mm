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
	NSMutableCharacterSet* allowedCharacters;
	
	et::Framebuffer::Pointer _mainFramebuffer;
	et::Framebuffer::Pointer _multisampledFramebuffer;
	
	et::RenderContext* _rc;
	et::RenderContextNotifier* _rcNotifier;
	
	BOOL _keyboardAllowed;
	BOOL _multisampled;
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
		allowedCharacters = [NSMutableCharacterSet alphanumericCharacterSet];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet punctuationCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet symbolCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet symbolCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet whitespaceCharacterSet]];
		
#if (!ET_OBJC_ARC_ENABLED)
		[allowedCharacters retain];
#endif
		
		[self performInitializationWithParameters:params];
	}
	
	return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];
	delete _rcNotifier;
	
#if (!ET_OBJC_ARC_ENABLED)
    [_context release];
    [super dealloc];
#endif
}

- (void)performInitializationWithParameters:(const RenderContextParameters&)params
{
	_multisampled = params.multisamplingQuality == MultisamplingQuality_Best;
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
	
#if (ET_OBJC_ARC_ENABLED)
	_context = newContext;
#else
	[_context release];
	_context = [newContext retain];
#endif
	
	[EAGLContext setCurrentContext:_context];
	[self createFramebuffer];
}

- (void)beginRender
{
    [EAGLContext setCurrentContext:_context];
	_rc->renderState().bindDefaultFramebuffer();
}

- (void)endRender
{
	checkOpenGLError("endRender");
	
	const GLenum discards[]  = { GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0};
	
	if (_multisampled)
	{
		_rc->renderState().bindFramebuffer(_multisampledFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, _mainFramebuffer->glID());
		glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, _multisampledFramebuffer->glID());
		glResolveMultisampleFramebufferAPPLE();
	}
	
	_rc->renderState().bindFramebuffer(_mainFramebuffer);
	_rc->renderState().bindRenderbuffer(_mainFramebuffer->colorRenderbuffer());
	
	glDiscardFramebufferEXT(GL_FRAMEBUFFER, (_multisampled ? 2 : 1), discards);
	
	[_context presentRenderbuffer:GL_RENDERBUFFER];
	checkOpenGLError("[_context presentRenderbuffer:GL_RENDERBUFFER]");
}

- (void)createFramebuffer
{
	self.contentScaleFactor = [[UIScreen mainScreen] scale];
	
	CAEAGLLayer* glLayer = (CAEAGLLayer*)self.layer;
	glLayer.contentsScale = self.contentScaleFactor;
	
	vec2i size(static_cast<int>(glLayer.bounds.size.width * glLayer.contentsScale),
		static_cast<int>(glLayer.bounds.size.height * glLayer.contentsScale));
	
	if (_mainFramebuffer.invalid())
	{
		_mainFramebuffer = _rc->framebufferFactory().createFramebuffer(size,
			"DefaultFramebuffer", 0, 0, 0, 0, 0, 0, true);
	}
	
	if (_multisampled && _multisampledFramebuffer.invalid())
	{
		_multisampledFramebuffer = _rc->framebufferFactory().createFramebuffer(size,
			"DefaultMultisampledFramebuffer", 0, 0, 0, 0, 0, 0, true);
	}

	if (_mainFramebuffer->colorRenderbuffer() == 0)
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		_mainFramebuffer->setColorRenderbuffer(buf);
	}
	
	if (_mainFramebuffer->depthRenderbuffer() == 0)
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		_mainFramebuffer->setDepthRenderbuffer(buf);
	}
	
	if (_multisampled && (_multisampledFramebuffer->colorRenderbuffer() == 0))
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		_multisampledFramebuffer->setColorRenderbuffer(buf);
	}
	
	if (_multisampled && (_multisampledFramebuffer->depthRenderbuffer() == 0))
	{
		GLuint buf = 0;
		glGenRenderbuffers(1, &buf);
		_multisampledFramebuffer->setDepthRenderbuffer(buf);
	}
	
	_rc->renderState().setDefaultFramebuffer([self defaultFramebuffer]);

	_rc->renderState().bindFramebuffer(_mainFramebuffer);
	_rc->renderState().bindRenderbuffer(_mainFramebuffer->colorRenderbuffer());
	if ([_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:glLayer])
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
			_mainFramebuffer->colorRenderbuffer());
		checkOpenGLError("glFramebufferRenderbuffer(...");
		
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &size.x);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &size.y);
	}
	
	_rc->renderState().bindRenderbuffer(_mainFramebuffer->depthRenderbuffer());
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size.x, size.y);
	checkOpenGLError("glRenderbufferStorage -> depth");
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
							  _mainFramebuffer->depthRenderbuffer());
	
	_mainFramebuffer->checkStatus();
	_mainFramebuffer->forceSize(size);
	
	if (_multisampled)
	{
		_rc->renderState().bindFramebuffer(_multisampledFramebuffer);
		
		/*
		 * Create color buffer
		 */
		_rc->renderState().bindRenderbuffer(_multisampledFramebuffer->colorRenderbuffer());
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_RGBA8_OES, size.x, size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
			_multisampledFramebuffer->colorRenderbuffer());
		checkOpenGLError("Multisampled color buffer");
		
		/*
		 * Create depth buffer
		 */
		_rc->renderState().bindRenderbuffer(_multisampledFramebuffer->depthRenderbuffer());
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT16, size.x, size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
			_multisampledFramebuffer->depthRenderbuffer());
		checkOpenGLError("Multisampled depth buffer");
		
		_multisampledFramebuffer->forceSize(size);
		_multisampledFramebuffer->checkStatus();
	}
	
	_rcNotifier->resized(size, _rc);
}

- (void)deleteFramebuffer
{
	_mainFramebuffer.reset(nullptr);
	_multisampledFramebuffer.reset(nullptr);
}

- (void)layoutSubviews
{
	[self createFramebuffer];
}

- (const Framebuffer::Pointer&)defaultFramebuffer
{
	return _multisampled ? _multisampledFramebuffer : _mainFramebuffer;
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
		pt.timestamp = static_cast<float>(touch.timestamp);
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		Input::PointerInputSource().pointerPressed(pt);
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
		pt.timestamp = static_cast<float>(touch.timestamp);
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		Input::PointerInputSource().pointerMoved(pt);
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
		pt.timestamp = static_cast<float>(touch.timestamp);
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		Input::PointerInputSource().pointerReleased(pt);
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
		pt.timestamp = static_cast<float>(touch.timestamp);
		pt.type = PointerType_General;
		
		float nx = 2.0f * pt.pos.x / ownSize.width - 1.0f;
		float ny = 1.0f - 2.0f * pt.pos.y / ownSize.height;
		pt.normalizedPos = vec2(nx, ny);
		Input::PointerInputSource().pointerCancelled(pt);
	}
}

- (void)onNotificationRecevied:(NSNotification*)notification
{
	if ([notification.name isEqualToString:etKeyboardRequiredNotification])
	{
		_keyboardAllowed = YES;
		[self performSelectorOnMainThread:@selector(becomeFirstResponder) withObject:nil waitUntilDone:NO];
	}
	else if ([notification.name isEqualToString:etKeyboardNotRequiredNotification])
	{
		[self performSelectorOnMainThread:@selector(resignFirstResponder) withObject:nil waitUntilDone:NO];
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

- (void)insertText:(NSString*)text
{
	if ([text length] == 1)
	{
		unichar character = [text characterAtIndex:0];
		if (character == ET_NEWLINE)
			Input::KeyboardInputSource().keyPressed(ET_KEY_RETURN);
	}

	NSString* filteredString = [text stringByTrimmingCharactersInSet:[allowedCharacters invertedSet]];
	if ([filteredString length] > 0)
	{
		std::string cString([filteredString cStringUsingEncoding:NSUTF8StringEncoding]);
		Input::KeyboardInputSource().charactersEntered(cString);
	}
}

- (void)deleteBackward
{
	Input::KeyboardInputSource().keyPressed(ET_KEY_BACKSPACE);
}

@end
