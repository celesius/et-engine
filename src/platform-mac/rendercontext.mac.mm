/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/opengl/openglcaps.h>
#include <et/input/input.h>
#include <et/app/applicationnotifier.h>
#include <et/rendering/rendercontext.h>

using namespace et;

class et::RenderContextPrivate
{
public:
	RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params);
	~RenderContextPrivate();
	
	void displayLinkSynchronized();
	void run();
	
public:
	RenderContext* _rc;
	NSWindow* _mainWindow;
	NSOpenGLPixelFormat* _pixelFormat;
	NSOpenGLContext* _openGlContext;
	NSOpenGLView* _openGlView;
	CVDisplayLinkRef _displayLink;
};

@interface etOpenGLView : NSOpenGLView
{
	Input::PointerInputSource _pointerInputSource;
}
@end

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params),
	_app(app), _programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
	_renderer(0), _screenScaleFactor(1)
{
	_private = new RenderContextPrivate(this, params);
	_renderState.setMainViewportSize(params.contextSize);
	
	ogl_caps().checkCaps();
	
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory);
	_programFactory = new ProgramFactory(this);
	_vertexBufferFactory = new VertexBufferFactory(_renderState);
}

RenderContext::~RenderContext()
{
	delete _private;
}

void RenderContext::init()
{
	_renderer = new Renderer(this);
	
	_fpsTimer.expired.connect(this, &RenderContext::onFPSTimerExpired);
	_fpsTimer.start(mainTimerPool(), 1.0f, -1);
	
	_private->run();
}

size_t RenderContext::renderingContextHandle()
{
	return 0;
}

void RenderContext::beginRender()
{
	[_private->_openGlContext makeCurrentContext];
	
	OpenGLCounters::reset();
	checkOpenGLError("RenderContext::beginRender");
}

void RenderContext::endRender()
{
	checkOpenGLError("RenderContext::endRender");

	[_private->_openGlContext flushBuffer];
	
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
 * RenderContextPrivate
 */
CVReturn cvDisplayLinkOutputCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime,
									 CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
	RenderContextPrivate* notifier = reinterpret_cast<RenderContextPrivate*>(displayLinkContext);
	notifier->displayLinkSynchronized();
	return kCVReturnSuccess;
}

RenderContextPrivate::RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params) : _rc(rc),
	_mainWindow(0), _pixelFormat(0), _openGlContext(0), _openGlView(0), _displayLink(0)
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
			0
		};

	_pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
	
	_mainWindow = [[NSWindow alloc] initWithContentRect:contentRect styleMask:NSTitledWindowMask | NSClosableWindowMask
												backing:NSBackingStoreBuffered defer:YES];
	
	_openGlView = [[etOpenGLView alloc] initWithFrame:openglRect pixelFormat:_pixelFormat];
	_openGlContext = [_openGlView openGLContext];
	
	[_mainWindow setContentView:_openGlView];
	[_mainWindow makeKeyAndOrderFront:NSApp];
}

RenderContextPrivate::~RenderContextPrivate()
{
	
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
}

@implementation etOpenGLView

- (void)mouseDown:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np = p / vec2(ownFrame.size.width, ownFrame.size.height);

	_pointerInputSource.pointerPressed(PointerInputInfo(PointerType_General, p, np, 0, 1, [theEvent timestamp]));
}

- (void)mouseUp:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np = p / vec2(ownFrame.size.width, ownFrame.size.height);
	
	_pointerInputSource.pointerReleased(PointerInputInfo(PointerType_General, p, np, 0, 1, [theEvent timestamp]));
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np = p / vec2(ownFrame.size.width, ownFrame.size.height);
	
	_pointerInputSource.pointerMoved(PointerInputInfo(PointerType_General, p, np, 0, 1, [theEvent timestamp]));
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np = p / vec2(ownFrame.size.width, ownFrame.size.height);
	
	_pointerInputSource.pointerMoved(PointerInputInfo(PointerType_General, p, np, 0, 1, [theEvent timestamp]));
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	vec2 p(nativePoint.x, ownFrame.size.height - nativePoint.y);
	vec2 np = p / vec2(ownFrame.size.width, ownFrame.size.height);
	
	_pointerInputSource.pointerScrolled(PointerInputInfo(PointerType_General, p, np, [theEvent deltaY], [theEvent hash], [theEvent timestamp]));
}

@end