/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/opengl/openglcaps.h>
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

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params),
	_app(app), _programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
	_renderer(0)
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
		{ NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core, 0 };
	
	_pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes] autorelease];
	
	_mainWindow = [[NSWindow alloc] initWithContentRect:contentRect styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
	_openGlView = [[NSOpenGLView alloc] initWithFrame:openglRect pixelFormat:_pixelFormat];
	_openGlContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:nil];
	
	[_openGlView setOpenGLContext:_openGlContext];
	
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
		CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, static_cast<CGLContextObj>([_openGlContext CGLContextObj]),
														  static_cast<CGLPixelFormatObj>([_pixelFormat CGLPixelFormatObj]));
	}
	
	CVDisplayLinkStart(_displayLink);
}

void RenderContextPrivate::displayLinkSynchronized()
{
	[_openGlContext makeCurrentContext];
	[_openGlContext flushBuffer];
}