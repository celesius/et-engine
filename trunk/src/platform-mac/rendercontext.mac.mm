/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <AppKit/NSWindow.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSScreen.h>
#include <AppKit/NSMenu.h>
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
}
@end

@interface etOpenGLView : NSOpenGLView
{
@public
	Input::PointerInputSource pointerInputSource;
	ApplicationNotifier applicationNotifier;
}

@end

class et::RenderContextPrivate
{
public:
	RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params);
	~RenderContextPrivate();
	
	void displayLinkSynchronized();
	void run();
	
public:
	RenderContext* _rc;
	etWindowDelegate* _windowDelegate;
	etOpenGLView* _openGlView;
	
	NSWindow* _mainWindow;
	NSOpenGLPixelFormat* _pixelFormat;
	NSOpenGLContext* _openGlContext;
	CVDisplayLinkRef _displayLink;
	
	bool firstSync;
};

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params),
	_app(app), _programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
	_renderer(0), _screenScaleFactor(1)
{
	_private = new RenderContextPrivate(this, params);
	
	openGLCapabilites().checkCaps();
	_renderState.setMainViewportSize(params.contextSize);
	
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory);
	_programFactory = new ProgramFactory(this);
	_vertexBufferFactory = new VertexBufferFactory(_renderState);

	_renderer = new Renderer(this);
	
	_fpsTimer.expired.connect(this, &RenderContext::onFPSTimerExpired);
}

RenderContext::~RenderContext()
{
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
	OpenGLCounters::reset();
	checkOpenGLError("RenderContext::beginRender");
}

void RenderContext::endRender()
{
	checkOpenGLError("RenderContext::endRender");

	++_info.averageFramePerSecond;
	_info.averageDIPPerSecond += OpenGLCounters::DIPCounter;
	_info.averagePolygonsPerSecond += OpenGLCounters::primitiveCounter;
}

void RenderContext::onFPSTimerExpired(NotifyTimer*)
{
	if (_info.averageFramePerSecond > 0)
	{
		_info.averageDIPPerSecond /= _info.averageFramePerSecond;
		_info.averagePolygonsPerSecond /= _info.averageFramePerSecond;
	}
	
	renderingInfoUpdated.invoke(_info);
	
	_info.averageFramePerSecond = 0;
	_info.averageDIPPerSecond = 0;
}

void RenderContext::resized(const vec2i& sz)
{
	_renderState.setMainViewportSize(sz);
}

void RenderContext::updateScreenScale(const vec2i& screenSize)
{
	size_t newScale = (screenSize.x - 1) / (3 * _params.baseContextSize.x / 2) + 1;
	if (newScale == _screenScaleFactor) return;
	
	_screenScaleFactor = newScale;
	screenScaleFactorChanged.invoke(_screenScaleFactor);
}

/*
 *
 * RenderContextPrivate
 *
 */
CVReturn cvDisplayLinkOutputCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow,
	const CVTimeStamp *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
	@autoreleasepool
	{
		RenderContextPrivate* notifier = reinterpret_cast<RenderContextPrivate*>(displayLinkContext);
		notifier->displayLinkSynchronized();
		return kCVReturnSuccess;
	}
}

RenderContextPrivate::RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params) : _rc(rc),
	_mainWindow(0), _pixelFormat(0), _openGlContext(0), _openGlView(0), _displayLink(0), firstSync(true)
{
	NSRect screenRect = [[NSScreen mainScreen] frame];
	
	NSRect contentRect = NSMakeRect(0.5f * (screenRect.size.width - params.contextSize.x),
									0.5f * (screenRect.size.height - params.contextSize.y),
									params.contextSize.x, params.contextSize.y);
	
	NSRect openglRect = NSMakeRect(0.0f, 0.0f, params.contextSize.x, params.contextSize.y);
	
	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
		{
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
			
			NSOpenGLPFAColorSize, 24,
			NSOpenGLPFAAlphaSize, 8,
			NSOpenGLPFADepthSize, 32,
			
			NSOpenGLPFAAccelerated,
			NSOpenGLPFADoubleBuffer,

			NSOpenGLPFAMultisample,
			NSOpenGLPFASampleBuffers, 4,
			NSOpenGLPFASamples, 16,

			0
		};

	_pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
	assert(_pixelFormat != nil);
	
	_mainWindow = [[NSWindow alloc] initWithContentRect:contentRect
											  styleMask:NSTitledWindowMask | NSClosableWindowMask
												backing:NSBackingStoreBuffered defer:YES];
	
	_windowDelegate = [etWindowDelegate new];
	[_mainWindow setDelegate:_windowDelegate];
	
	_openGlView = [[etOpenGLView alloc] initWithFrame:openglRect pixelFormat:_pixelFormat];
	_openGlContext = [[_openGlView openGLContext] retain];
	
	[_openGlContext makeCurrentContext];
	
	[_mainWindow setContentView:_openGlView];
	[_mainWindow makeKeyAndOrderFront:NSApp];
}

RenderContextPrivate::~RenderContextPrivate()
{
	[_mainWindow release];
	[_openGlView release];
	[_openGlContext release];
	[_windowDelegate release];
}

void RenderContextPrivate::run()
{
	if (_displayLink == nil)
	{
		CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
		CVDisplayLinkSetOutputCallback(_displayLink, cvDisplayLinkOutputCallback, this);
		CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink,
			static_cast<CGLContextObj>([_openGlContext CGLContextObj]),
			static_cast<CGLPixelFormatObj>([_pixelFormat CGLPixelFormatObj]));
	}
	
	CVDisplayLinkStart(_displayLink);
}

void RenderContextPrivate::displayLinkSynchronized()
{
	if (firstSync)
	{
		ThreadId currentThread = Threading::currentThread();
		Threading::setMainThread(currentThread);
		Threading::setRenderingThread(currentThread);
		firstSync = false;
	}
	
	if (application().active())
	{
		CGLLockContext(reinterpret_cast<CGLContextObj>([_openGlContext CGLContextObj]));
		[_openGlContext makeCurrentContext];
		
		_windowDelegate->applicationNotifier.notifyIdle();
		
		[_openGlContext flushBuffer];
		CGLUnlockContext(reinterpret_cast<CGLContextObj>([_openGlContext CGLContextObj]));
	}
}

@implementation etOpenGLView

- (PointerInputInfo)mousePointerInfo:(NSEvent*)theEvent withType:(PointerType)type;
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np(2.0f * p.x / ownFrame.size.width - 1.0f, 1.0f - 2.0f * p.y / ownFrame.size.height);
	
	return PointerInputInfo(type, p, np, 0, [theEvent eventNumber], [theEvent timestamp]);
}

- (void)mouseDown:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_General];
	pointerInputSource.pointerPressed(info);
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_General];
	pointerInputSource.pointerMoved(info);
}

- (void)mouseUp:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_General];
	pointerInputSource.pointerReleased(info);
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_RightButton];
	pointerInputSource.pointerPressed(info);
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_RightButton];
	pointerInputSource.pointerMoved(info);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	PointerInputInfo info = [self mousePointerInfo:theEvent withType:PointerType_RightButton];
	pointerInputSource.pointerReleased(info);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np(2.0f * p.x / ownFrame.size.width - 1.0f, 1.0f - 2.0f * p.y / ownFrame.size.height);
	
	pointerInputSource.pointerScrolled(PointerInputInfo(PointerType_General, p, np,
		[theEvent deltaY], [theEvent hash], [theEvent timestamp]));
}

@end

@implementation etWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
	applicationNotifier.notifyDeactivated();
	return YES;
}

@end