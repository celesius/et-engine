/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/opengl/opengl.h>
#include <et/rendering/rendercontext.h>
#include <et/app/application.h>
#include <et/rendering/renderer.h>
#include <et/opengl/openglcaps.h>
#include <et/platform-ios/openglviewcontroller.h>

using namespace et;

class et::RenderContextPrivate
{
public:
	RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params);
	~RenderContextPrivate();
	
	bool initOpenGL(const RenderContextParameters& params);
	
public:
#if !defined(ET_EMBEDDED_APPLICATION)	
	etOpenGLViewController* _viewController = nil;
#endif	
	
	RenderContext* _rc = nullptr;
	bool failed = false;
	uint64_t frameDuration = 0;
};

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params),
	_app(app), _programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
	_renderer(0)
{
	_private = new RenderContextPrivate(this, params);
	if (_private->failed)
	{
		delete _private;
		_private = 0;
		application().quit(1);
		return;
	}
    
    openGLCapabilites().checkCaps();

	_renderState.setRenderContext(this);
	_programFactory = new ProgramFactory(this);
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory.ptr());
	_vertexBufferFactory = new VertexBufferFactory(_renderState);
	
	CGSize screenSize = [[[UIApplication sharedApplication] delegate] window].frame.size;
	float screenScale = [[UIScreen mainScreen] scale];
	
	vec2i contextSize(static_cast<int>(screenScale * screenSize.width),
		static_cast<int>(screenScale * screenSize.height));

	updateScreenScale(contextSize);
	_renderState.setMainViewportSize(contextSize);
}

RenderContext::~RenderContext()
{
	delete _private;
}

void RenderContext::init()
{
	_renderer = new Renderer(this);
	
#if !defined(ET_EMBEDDED_APPLICATION)	
	[_private->_viewController setRenderContext:this];
#endif
	
	_fpsTimer.expired.connect(this, &RenderContext::onFPSTimerExpired);
	_fpsTimer.start(mainTimerPool(), 1.0f, NotifyTimer::RepeatForever);
}

size_t RenderContext::renderingContextHandle()
{
#if defined(ET_EMBEDDED_APPLICATION)	
	return 0;
#else	
	return reinterpret_cast<size_t>(_private->_viewController);
#endif	
}

void RenderContext::beginRender()
{
	OpenGLCounters::reset();
	_private->frameDuration = queryCurrentTimeInMicroSeconds();
	
#if !defined(ET_EMBEDDED_APPLICATION)	
	[_private->_viewController beginRender];
	checkOpenGLError("RenderContext::beginRender");
#endif	
}

void RenderContext::endRender()
{
#if !defined(ET_EMBEDDED_APPLICATION)	
	[_private->_viewController endRender];
	checkOpenGLError("RenderContext::endRender");
#endif
	
	_info.averageDIPPerSecond += OpenGLCounters::DIPCounter;
	_info.averagePolygonsPerSecond += OpenGLCounters::primitiveCounter;
	
	++_info.averageFramePerSecond;
	_info.averageFrameTimeInMicroseconds += queryCurrentTimeInMicroSeconds() - _private->frameDuration;
}

/*
 * RenderContextPrivate
 */
RenderContextPrivate::RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params) :
	_rc(rc), failed(false)
{
	failed = !initOpenGL(params);
}

bool RenderContextPrivate::initOpenGL(const RenderContextParameters& params)
{
#if defined(ET_EMBEDDED_APPLICATION)	
	return true;
#else
	_viewController = [[etOpenGLViewController alloc] initWithParameters:params];
	return (_viewController != nil);
#endif	
}

RenderContextPrivate::~RenderContextPrivate()
{
#if (!defined(ET_EMBEDDED_APPLICATION)) && (!ET_OBJC_ARC_ENABLED)
	[_viewController release];
#endif	
}
