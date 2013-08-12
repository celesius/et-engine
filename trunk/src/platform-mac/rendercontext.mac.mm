/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <AppKit/NSWindow.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSScreen.h>
#include <AppKit/NSMenu.h>
#include <AppKit/NSTrackingArea.h>
#include <CoreVideo/CVDisplayLink.h>

#include <et/opengl/openglcaps.h>
#include <et/input/input.h>
#include <et/app/applicationnotifier.h>
#include <et/threading/threading.h>
#include <et/rendering/rendercontext.h>

using namespace et;

@interface etWindowDelegate : NSObject<NSWindowDelegate>
{
@public
	ApplicationNotifier applicationNotifier;
	RenderContextPrivate* rcPrivate;
}
@end

@interface etOpenGLWindow : NSWindow
@end

@interface etOpenGLView : NSOpenGLView
{
@public
	NSTrackingArea* _trackingArea;
	Input::PointerInputSource pointerInputSource;
	Input::GestureInputSource gestureInputSource;
	ApplicationNotifier applicationNotifier;
	RenderContextPrivate* rcPrivate;
}

@end

class et::RenderContextPrivate
{
public:
	RenderContextPrivate(RenderContext*, RenderContextParameters&, const ApplicationParameters&);
	~RenderContextPrivate();
	
	int displayLinkSynchronized();
	
	void run();
	void resize(const NSSize&);
	void render();
	void stop();
		
	bool canPerformOperations()
		{ return !firstSync && (displayLink != nil); }
	
private:
	etWindowDelegate* windowDelegate;
	etOpenGLView* openGlView;
	etOpenGLWindow* mainWindow;
	
	NSOpenGLPixelFormat* pixelFormat;
	NSOpenGLContext* openGlContext;
	CVDisplayLinkRef displayLink;
	
	bool firstSync;
};

RenderContext::RenderContext(const RenderContextParameters& inParams, Application* app) : _params(inParams),
	_app(app), _programFactory(nullptr), _textureFactory(nullptr), _framebufferFactory(nullptr),
	_vertexBufferFactory(nullptr), _renderer(nullptr), _screenScaleFactor(1)
{
	_private = new RenderContextPrivate(this, _params, app->parameters());
	
	openGLCapabilites().checkCaps();
	updateScreenScale(_params.contextSize);
	
	_renderState.setMainViewportSize(_params.contextSize);
	
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory.ptr());
	_programFactory = new ProgramFactory(this);
	_vertexBufferFactory = new VertexBufferFactory(_renderState);

	_renderer = new Renderer(this);
	
	ET_CONNECT_EVENT(_fpsTimer.expired, RenderContext::onFPSTimerExpired)
}

RenderContext::~RenderContext()
{
	_textureFactory.release();
	_vertexBufferFactory.release();
	_programFactory.release();
	_framebufferFactory.release();
	delete _private;
}

void RenderContext::init()
{
	_fpsTimer.start(mainTimerPool(), 1.0f, -1);
	_private->run();
}

size_t RenderContext::renderingContextHandle()
{
	return 0;
}

void RenderContext::beginRender()
{
	checkOpenGLError("RenderContext::beginRender");
	
	OpenGLCounters::reset();
	_renderState.bindDefaultFramebuffer();
}

void RenderContext::endRender()
{
	checkOpenGLError("RenderContext::endRender");

	++_info.averageFramePerSecond;
	_info.averageDIPPerSecond += OpenGLCounters::DIPCounter;
	_info.averagePolygonsPerSecond += OpenGLCounters::primitiveCounter;
}

/*
 *
 * RenderContextPrivate
 *
 */
CVReturn cvDisplayLinkOutputCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*,
	CVOptionFlags, CVOptionFlags*, void *displayLinkContext);

RenderContextPrivate::RenderContextPrivate(RenderContext*, RenderContextParameters& params,
	const ApplicationParameters& appParams) : mainWindow(nil), pixelFormat(nil),
	openGlContext(nil), openGlView(nil), displayLink(nil), firstSync(true)
{
	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
	{
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 32,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAAcceleratedCompute,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFASampleBuffers, 4,
		NSOpenGLPFASamples, 16,
		NSOpenGLPFAMultisample,
		0,
		0,
		0,
	};
	
	size_t lastEntry = (sizeof(pixelFormatAttributes) / sizeof(NSOpenGLPixelFormatAttribute)) - 3;
	if (params.openGLForwardContext)
	{
		pixelFormatAttributes[lastEntry++] = NSOpenGLPFAOpenGLProfile;
		pixelFormatAttributes[lastEntry++] = NSOpenGLProfileVersion3_2Core;
	}
	
	pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
	assert(pixelFormat != nil);
	
	NSUInteger windowMask = NSBorderlessWindowMask;
	
	if (appParams.windowSize != WindowSize_Fullscreen)
	{
		if ((appParams.windowStyle & WindowStyle_Caption) == WindowStyle_Caption)
			windowMask |= NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
		
		if ((appParams.windowStyle & WindowStyle_Sizable) == WindowStyle_Sizable)
			windowMask |= NSResizableWindowMask;
	}
	
	NSScreen* mainScreen = [NSScreen mainScreen];
	NSRect visibleRect = [mainScreen visibleFrame];
	
	NSRect contentRect = { };
	if (appParams.windowSize == WindowSize_FillWorkarea)
	{
		contentRect = [NSWindow contentRectForFrameRect:visibleRect styleMask:windowMask];
	}
	else if (appParams.windowSize == WindowSize_Fullscreen)
	{
		[[NSApplication sharedApplication] setPresentationOptions:NSApplicationPresentationAutoHideDock |
			NSApplicationPresentationAutoHideMenuBar | NSApplicationPresentationFullScreen];
		
		windowMask |= NSFullScreenWindowMask;
		contentRect = [mainScreen frame];
	}
	else
	{
		contentRect = NSMakeRect(0.5f * (visibleRect.size.width - params.contextSize.x),
			visibleRect.origin.y + 0.5f * (visibleRect.size.height - params.contextSize.y),
			params.contextSize.x, params.contextSize.y);
	}
	
	params.contextSize = vec2i(static_cast<int>(contentRect.size.width), static_cast<int>(contentRect.size.height));
	
	mainWindow = [[etOpenGLWindow alloc] initWithContentRect:contentRect
		styleMask:windowMask backing:NSBackingStoreBuffered defer:YES];
	
	windowDelegate = [[etWindowDelegate alloc] init];
	windowDelegate->rcPrivate = this;
	[mainWindow setDelegate:windowDelegate];
	[mainWindow setOpaque:YES];
	
	openGlView = [[etOpenGLView alloc] initWithFrame:NSMakeRect(0.0f, 0.0f, contentRect.size.width,
		contentRect.size.height) pixelFormat:pixelFormat];
	openGlView->rcPrivate = this;
	[openGlView setWantsBestResolutionOpenGLSurface:YES];
	
	openGlContext = [[openGlView openGLContext] retain];
	[openGlContext makeCurrentContext];
	
	if (appParams.windowSize == WindowSize_Fullscreen)
	{
		[mainWindow setHidesOnDeactivate:YES];
		[mainWindow setLevel:NSMainMenuWindowLevel + 1];
		[openGlContext setFullScreen];
	}
	
	[mainWindow setContentView:openGlView];
	[mainWindow makeKeyAndOrderFront:[NSApplication sharedApplication]];
}

RenderContextPrivate::~RenderContextPrivate()
{
	[openGlView release];
	[openGlContext release];
	[windowDelegate release];
}

void RenderContextPrivate::run()
{
	if (displayLink == nil)
	{
		CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
		CVDisplayLinkSetOutputCallback(displayLink, cvDisplayLinkOutputCallback, this);
		CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,
			static_cast<CGLContextObj>([openGlContext CGLContextObj]),
			static_cast<CGLPixelFormatObj>([pixelFormat CGLPixelFormatObj]));
	}
	
	CVDisplayLinkStart(displayLink);
}

void RenderContextPrivate::stop()
{
	CVDisplayLinkStop(displayLink);
	CVDisplayLinkRelease(displayLink);
	displayLink = nil;
}

void RenderContextPrivate::render()
{
	CGLContextObj glObject = reinterpret_cast<CGLContextObj>([openGlContext CGLContextObj]);
	[openGlContext makeCurrentContext];
	CGLLockContext(glObject);
	windowDelegate->applicationNotifier.notifyIdle();
	CGLFlushDrawable(glObject);
	CGLUnlockContext(glObject);
}

int RenderContextPrivate::displayLinkSynchronized()
{
	if (firstSync)
	{
		ThreadId currentThread = Threading::currentThread();
		Threading::setMainThread(currentThread);
		Threading::setRenderingThread(currentThread);
		firstSync = false;
	}
	
	if (application().running() && !application().suspended())
		render();

	return kCVReturnSuccess;
}

void RenderContextPrivate::resize(const NSSize& sz)
{
	CGLContextObj glObject = reinterpret_cast<CGLContextObj>([openGlContext CGLContextObj]);
	
	[openGlContext makeCurrentContext];
	CGLLockContext(glObject);
	
	windowDelegate->applicationNotifier.notifyResize(vec2i(static_cast<int>(sz.width),
		static_cast<int>(sz.height)));

	CGLUnlockContext(glObject);
}

/*
 * Display link callback
 */
CVReturn cvDisplayLinkOutputCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*,
	CVOptionFlags, CVOptionFlags*, void *displayLinkContext)
{
	@autoreleasepool
	{
		return reinterpret_cast<RenderContextPrivate*>(displayLinkContext)->displayLinkSynchronized();
	}
}

/*
 * OpenGL View implementation
 */
@implementation etOpenGLView

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
	self = [super initWithFrame:frameRect pixelFormat:format];
	if (self)
	{
	}
	return self;
}

- (PointerInputInfo)mousePointerInfo:(NSEvent*)theEvent withType:(PointerType)type;
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np(2.0f * p.x / ownFrame.size.width - 1.0f, 1.0f - 2.0f * p.y / ownFrame.size.height);

	return PointerInputInfo(type, p, np, vec2(0.0f), [theEvent eventNumber],
		static_cast<float>([theEvent timestamp]), PointerOrigin_Any);
}

- (void)mouseDown:(NSEvent *)theEvent
{
	pointerInputSource.pointerPressed([self mousePointerInfo:theEvent withType:PointerType_General]);
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	pointerInputSource.pointerMoved([self mousePointerInfo:theEvent withType:PointerType_None]);
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	pointerInputSource.pointerMoved([self mousePointerInfo:theEvent withType:PointerType_General]);
}

- (void)mouseUp:(NSEvent *)theEvent
{
	pointerInputSource.pointerReleased([self mousePointerInfo:theEvent withType:PointerType_General]);
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	pointerInputSource.pointerPressed([self mousePointerInfo:theEvent withType:PointerType_RightButton]);
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	pointerInputSource.pointerMoved([self mousePointerInfo:theEvent withType:PointerType_RightButton]);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	pointerInputSource.pointerReleased([self mousePointerInfo:theEvent withType:PointerType_RightButton]);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	NSPoint nativePoint = [theEvent locationInWindow];

	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np(2.0f * p.x / ownFrame.size.width - 1.0f, 1.0f - 2.0f * p.y / ownFrame.size.height);
	vec2 scroll([theEvent deltaX] / ownFrame.size.width, [theEvent deltaY] / ownFrame.size.height);

	PointerOrigin origin = (([theEvent momentumPhase] != NSEventPhaseNone) ||
		([theEvent phase] != NSEventPhaseNone)) ? PointerOrigin_Trackpad : PointerOrigin_Mouse;

	pointerInputSource.pointerScrolled(PointerInputInfo(PointerType_General, p, np,
		scroll, [theEvent hash], static_cast<float>([theEvent timestamp]), origin));
}

- (void)beginGestureWithEvent:(NSEvent *)event
{
    (void)event;
}

- (void)magnifyWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(
		GestureInputInfo(GestureTypeMask_Zoom, event.magnification));
}

- (void)swipeWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(
		GestureInputInfo(GestureTypeMask_Swipe, event.deltaX, event.deltaY));
}

- (void)rotateWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(
		GestureInputInfo(GestureTypeMask_Rotate, event.rotation));
}

- (void)endGestureWithEvent:(NSEvent *)event
{
    (void)event;
}

- (void)keyDown:(NSEvent *)event
{
    (void)event;
}

- (void)keyUp:(NSEvent *)event
{
    (void)event;
}

- (void)drawRect:(NSRect)dirtyRect
{
	(void)dirtyRect;
	if (rcPrivate->canPerformOperations())
		rcPrivate->render();
}

- (void)reshape
{
	[super reshape];
	
	if (_trackingArea)
	{
		[self removeTrackingArea:_trackingArea];
		[_trackingArea release];
	}
	
	_trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
		options:NSTrackingMouseMoved | NSTrackingActiveAlways owner:self userInfo:nil];

	[self addTrackingArea:_trackingArea];
	
	if (rcPrivate->canPerformOperations())
		rcPrivate->resize(self.bounds.size);
}

@end

@implementation etOpenGLWindow : NSWindow

- (void)keyDown:(NSEvent*)theEvent
{
	unichar key = [theEvent.characters length] ? [theEvent.characters characterAtIndex:0] : 0;
	Input::KeyboardInputSource().keyPressed(static_cast<unsigned char>(key & 0xff));
}

@end

@implementation etWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
    (void)sender;
	applicationNotifier.notifySuspended();
	rcPrivate->stop();
	return YES;
}

@end
