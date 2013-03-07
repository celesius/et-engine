/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/platform-android/nativeactivity.h>

#include <et/app/application.h>
#include <et/opengl/openglcaps.h>
#include <et/rendering/rendercontext.h>

using namespace et;

class et::RenderContextPrivate
{
public:
	RenderContextPrivate();

public:
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

};

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params),
	_app(app), _programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
	_renderer(0)
{
	_private = new RenderContextPrivate();
	openGLCapabilites().checkCaps();

	_screenScaleFactor = 0;
	
	_renderState.setRenderContext(this);
	_programFactory = new ProgramFactory(this);
	_textureFactory = new TextureFactory(this);
	_framebufferFactory = new FramebufferFactory(this, _textureFactory);
	_vertexBufferFactory = new VertexBufferFactory(_renderState);
}

RenderContext::~RenderContext()
{
	delete _vertexBufferFactory;
	delete _framebufferFactory;
	delete _programFactory;
	delete _textureFactory;
	delete _renderer; 
	delete _private;
}

void RenderContext::init()
{
	_renderer = new Renderer(this);
}

bool RenderContext::valid()
{
	return _private != nullptr;
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
	eglSwapBuffers(_private->display, _private->surface);
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

void RenderContext::setActive(bool active)
{
	if (_app->running())
		_app->setActive(active);
}

void RenderContext::resized(const vec2i& sz)
{
	updateScreenScale(sz);
	_renderState.setMainViewportSize(sz);
	
	if (_app->running())
		_app->contextResized(sz);
}

void RenderContext::updateScreenScale(const vec2i& screenSize)
{
	size_t newScale = (screenSize.x - 1) / (3 * _params.baseContextSize.x / 2) + 1;
	if (newScale == _screenScaleFactor) return;

	_screenScaleFactor = newScale;
	screenScaleFactorChanged.invoke(_screenScaleFactor);
}


/**
 *
 * RenderContextPrivate
 *
 */

RenderContextPrivate::RenderContextPrivate()
{
    const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
    };
	
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	android_app* sharedApplication = reinterpret_cast<android_app*>(application().renderingContextHandle());
    ANativeWindow_setBuffersGeometry(sharedApplication->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, sharedApplication->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) return;

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
}