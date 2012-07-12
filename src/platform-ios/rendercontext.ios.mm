/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
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
	etOpenGLViewController* _viewController;
#endif	
	RenderContext* _rc;
	bool failed;
	
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
    
    ogl_caps().checkCaps();

	_renderState.setRenderContext(this);
	_programFactory = new ProgramFactory(this);
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory);
	_vertexBufferFactory = new VertexBufferFactory(_renderState);
	
	CGSize screenSize = [[[UIScreen mainScreen] currentMode] size];
	int w = static_cast<int>(screenSize.width);
	int h = static_cast<int>(screenSize.height);
	
	_screenScaleFactor = (w - 1) / (3 * params.baseContextSize.x / 2) + 1;
	_renderState.setMainViewportSize(vec2i(w, h));
}

RenderContext::~RenderContext()
{
	delete _renderer; 
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
	_info.averageFramePerSecond++;
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
	_info.averagePolygonsPerSecond = 0;
}

void RenderContext::resized(const vec2i& sz)
{
	updateScreenScale(sz);
	_renderState.setMainViewportSize(sz);
	_app->contextResized(sz);
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
RenderContextPrivate::RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params) : _rc(rc), failed(false)
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
#if !defined(ET_EMBEDDED_APPLICATION)	
	[_viewController release];
#endif	
}