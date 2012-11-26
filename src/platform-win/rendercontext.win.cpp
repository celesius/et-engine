/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <WinUser.h>

#include <et/opengl/opengl.h>
#include <et/app/application.h>
#include <et/rendering/rendercontext.h>
#include <et/rendering/renderer.h>
#include <et/opengl/openglcaps.h>
#include <et/input/input.h>

using namespace et;

LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct RenderContextData
{
	bool initialized;
	HWND hWnd;
	HDC hDC;
	HGLRC hGLRC;

	RenderContextData() : initialized(false), hWnd(0), hDC(0), hGLRC(0) { }
	RenderContextData& release();
};

class et::RenderContextPrivate : public Input::KeyboardInputSource, public Input::PointerInputSource
{
public:
	RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params);
	~RenderContextPrivate();

public:
	bool failed;
	HINSTANCE hInstance;
	WNDCLASSEX wndClass;
	vec2i windowSize;

	RenderContextData primaryContext;
	RenderContextData dummyContext;

	inline RenderContext* renderContext() 
		{ return _renderContext; }

	void setActive(bool active);
	void resized(const vec2i& sz);

	bool shouldPostMovementMessage(int x, int y);

private:
	HWND createWindow(WindowStyle, const vec2i&, const vec2i&);
	RenderContextData createDummyContext(HWND hWnd);

	bool initWindow(const RenderContextParameters& params);
	bool initOpenGL(const RenderContextParameters& params);

	int chooseMSAAPixelFormat(HDC, PIXELFORMATDESCRIPTOR*);
	int chooseAAPixelFormat(HDC, PIXELFORMATDESCRIPTOR*);
	int choosePixelFormat(HDC, PIXELFORMATDESCRIPTOR*);

private:
	RenderContext* _renderContext;
	int _mouseX;
	int _mouseY;
};

RenderContext::RenderContext(const RenderContextParameters& params, Application* app) : _params(params), _app(app),
	_programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0), _renderer(0)
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

	_screenScaleFactor = (_private->windowSize.x - 1) / (3 * _params.baseContextSize.x / 2) + 1;
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

	RECT r = { };
	GetClientRect(_private->primaryContext.hWnd, &r);
	_renderState.setMainViewportSize(vec2i(r.right - r.left, r.bottom - r.top));

	_fpsTimer.expired.connect(this, &RenderContext::onFPSTimerExpired);
	_fpsTimer.start(mainTimerPool(), 1.0f, -1);
}

size_t RenderContext::renderingContextHandle()
{
	return reinterpret_cast<size_t>(_private->primaryContext.hWnd);
}

void RenderContext::beginRender()
{
	OpenGLCounters::reset();

	checkOpenGLError("RenderContext::beginRender");
}

void RenderContext::endRender()
{
	checkOpenGLError("RenderContext::endRender");
	SwapBuffers(_private->primaryContext.hDC);

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
/*
	std::cout << _info.averageFramePerSecond << " fps, " 
				<< _info.averageDIPPerSecond << " dips, " 
				<< _info.averagePolygonsPerSecond << " polys." << std::endl;
*/
	renderingInfoUpdated.invoke(_info);

	_info.averageFramePerSecond = 0;
	_info.averageDIPPerSecond = 0;
}

void RenderContext::setActive(bool active)
{
	_app->setActive(active);
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
 *
 * RenderContextPrivate
 * 
 */

RenderContextPrivate::RenderContextPrivate(RenderContext* rc, const RenderContextParameters& params) : failed(false),
	hInstance(0), _renderContext(rc), _mouseX(-1), _mouseY(-1)
{
	failed = !initWindow(params);

	if (!failed)
		failed = !initOpenGL(params);
}

HWND RenderContextPrivate::createWindow(WindowStyle style, const vec2i& pos, const vec2i& size)
{ 
	bool isSizeable = style == WindowStyle_Sizable;
	bool hasCaption = style == (WindowStyle_FixedWithCaption) || isSizeable; 
	bool hasBorder = style == (WindowStyle_FixedWithoutCaption) || isSizeable;

	UINT wCaptionStyle = WS_MINIMIZEBOX | WS_CAPTION | WS_ACTIVECAPTION | (isSizeable ? WS_SIZEBOX | WS_MAXIMIZEBOX : 0);
	UINT wStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU | (hasCaption ? wCaptionStyle : 0) | (hasBorder ? WS_BORDER : 0);

	hasCaption |= hasBorder;

	vec2i vPos;
	vPos.x = pos.x * hasCaption;
	vPos.y = pos.y * hasCaption;
	vec2i winSize = size;

	tagRECT wRect; 
	wRect.left = pos.x;
	wRect.top  = pos.y;

	RECT wR = { };
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&wR, 0);

	int nScreenH = wR.bottom - wR.top;
	int nScreenW = wR.right  - wR.left;

	if (style == WindowStyle_Fullscreen)
	{
		nScreenW = GetSystemMetrics(SM_CXSCREEN);
		nScreenH = GetSystemMetrics(SM_CYSCREEN);
	}

	if (!hasCaption)
	{
		wRect.right  = nScreenW;
		wRect.bottom = nScreenH;
	} 
	else
	{
		wRect.right  = pos.x + size.x;
		wRect.bottom = pos.y + size.y;
		AdjustWindowRect(&wRect, wStyle, false);

		if (pos.x == -1)
			vPos.x = (nScreenW - wRect.right  + wRect.left) / 2;

		if (pos.y == -1)
			vPos.y = (nScreenH - wRect.bottom + wRect.top ) / 2;
	}

	winSize.x = wRect.right  - wRect.left;
	winSize.y = wRect.bottom - wRect.top;

	if (windowSize.square() == 0)
		windowSize = winSize;

	return CreateWindowExA(WS_EX_APPWINDOW, wndClass.lpszClassName, application().identifier().applicationName.c_str(), wStyle, vPos.x, vPos.y, 
		winSize.x, winSize.y, 0, 0, hInstance, 0);
}

bool RenderContextPrivate::initWindow(const RenderContextParameters& params)
{
	hInstance = GetModuleHandle(0);

	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = mainWndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(0, IDC_ARROW);
	wndClass.lpszClassName = "etWindowClass";

	if (!RegisterClassEx(&wndClass)) return false;

	primaryContext.hWnd = createWindow(params.windowStyle, params.contextPosition, params.contextSize);

	if (primaryContext.hWnd == 0) return false;

#if ET_PLATFORM_WIN64
	SetWindowLong(primaryContext.hWnd, GWLP_USERDATA, LONG(this));
#else
	SetWindowLong(primaryContext.hWnd, GWL_USERDATA, LONG(this));
#endif

	ShowWindow(primaryContext.hWnd, SW_SHOW);
	SetForegroundWindow(primaryContext.hWnd);
	SetFocus(primaryContext.hWnd);

	bool hasCaption = (params.windowStyle == WindowStyle_FixedWithCaption) || (params.windowStyle == WindowStyle_Sizable);
	bool isFullscreen = params.windowStyle == WindowStyle_Fullscreen;

	if (!hasCaption)
		SetCapture(primaryContext.hWnd);

	if (isFullscreen)
	{
		DEVMODE dm = { };
		EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm);
		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	return true;
}

RenderContextData RenderContextPrivate::createDummyContext(HWND hWnd)
{
	RenderContextData result;

	result.hWnd = hWnd;
	result.hDC = GetDC(result.hWnd);
	if (result.hDC == 0) return result;

	PIXELFORMATDESCRIPTOR dummy_pfd = { };
	dummy_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	dummy_pfd.nVersion = 1;
	dummy_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	dummy_pfd.cColorBits = 24;
	dummy_pfd.cAlphaBits = 8;
	dummy_pfd.cDepthBits = 24;

	int nPixelFormat = ChoosePixelFormat(result.hDC, &dummy_pfd); 

	if (nPixelFormat == 0)
		return result.release();

	if (!SetPixelFormat(result.hDC, nPixelFormat, &dummy_pfd))
		return result.release();

	result.hGLRC = wglCreateContext(result.hDC);
	if (result.hGLRC == 0)
		return result.release();

	if (!wglMakeCurrent(result.hDC, result.hGLRC))
		return result.release();

	result.initialized = true;
	return result;
}

bool RenderContextPrivate::initOpenGL(const RenderContextParameters& params)
{
	HWND dummyWindow = createWindow(WindowStyle_FixedWithoutCaption, vec2i(0), vec2i(0));
	if (dummyWindow == 0) return false;

	RenderContextData dummy = createDummyContext(dummyWindow);
	if (!dummy.initialized)	return false;

	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");

	size_t compositionFlag = 0;
	OSVERSIONINFO ver = { };
	ver.dwOSVersionInfoSize = sizeof(ver);
	if (GetVersionEx(&ver))
		compositionFlag = (ver.dwMajorVersion >= 6) ? PFD_SUPPORT_COMPOSITION : 0;

	PIXELFORMATDESCRIPTOR pfd = { };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | compositionFlag;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	int pixelFormat = 0;

	if (params.multisamplingQuality != MultisamplingQuality_None)
	{
		pixelFormat = chooseAAPixelFormat(dummy.hDC, &pfd);

		if (!pixelFormat)
			pixelFormat = choosePixelFormat(dummy.hDC, &pfd);
	}
	else
	{
		pixelFormat = choosePixelFormat(dummy.hDC, &pfd);
	}

	dummy.release();
	DestroyWindow(dummyWindow);
	if (pixelFormat == 0) return false;

	primaryContext.hDC = GetDC(primaryContext.hWnd);
	if (primaryContext.hDC == 0) return false;

	if (!SetPixelFormat(primaryContext.hDC, pixelFormat, &pfd))
	{  
		pixelFormat = choosePixelFormat(primaryContext.hDC, &pfd);
		if (!SetPixelFormat(primaryContext.hDC, pixelFormat, &pfd))
		{
			primaryContext.release();
			return false;
		}
	}

	if (!wglCreateContextAttribsARB)
	{
		primaryContext.hGLRC = wglCreateContext(primaryContext.hDC);
		if (primaryContext.hGLRC == 0)
		{
			primaryContext.release();
			return 0;
		}

		if (!wglMakeCurrent(primaryContext.hDC, primaryContext.hGLRC))
		{
			primaryContext.release();
			return false;
		}
	} 
	else
	{
		int attrib_list[] = 
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 
				params.openGLMaxVersion.x,

			WGL_CONTEXT_MINOR_VERSION_ARB, 
				params.openGLMaxVersion.y,

			WGL_CONTEXT_FLAGS_ARB, 
				params.openGLForwardContext * WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,

			0, 0, 0, 0
		};

		if (params.openGLMaxVersion.x >= 3)
		{
			attrib_list[6] = WGL_CONTEXT_PROFILE_MASK_ARB;

			attrib_list[7] = params.openGLCoreProfile * WGL_CONTEXT_CORE_PROFILE_BIT_ARB | 
							 params.openGLCompatibilityProfile * WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}

		primaryContext.hGLRC = wglCreateContextAttribsARB(primaryContext.hDC, 0, attrib_list);
		if (primaryContext.hGLRC == 0)
		{
			DWORD lastError = GetLastError();
			if (lastError == ERROR_INVALID_VERSION_ARB)
				std::cout << "Error creating context: ERROR_INVALID_VERSION_ARB. Requested: " << params.openGLMaxVersion << std::endl;
			else if (lastError == ERROR_INVALID_PROFILE_ARB)
				std::cout << "Error creating context: ERROR_INVALID_PROFILE_ARB" << std::endl;
			else if (lastError == ERROR_INVALID_PROFILE_ARB)
				std::cout << "Error creating context: ERROR_INVALID_PROFILE_ARB" << std::endl;
		}

		while (primaryContext.hGLRC == 0)
		{
			if (attrib_list[3] == 0)
			{
				attrib_list[1] -= 1;
				attrib_list[3] = 9;
			}
			else
			{
				attrib_list[3] -= 1;
			}

			if (attrib_list[1] <= 0)
			{
				primaryContext.release();
				return 0;
			}

			primaryContext.hGLRC = wglCreateContextAttribsARB(primaryContext.hDC, 0, attrib_list);
			if (primaryContext.hGLRC == 0)
			{
				DWORD lastError = GetLastError();
				if (lastError == ERROR_INVALID_VERSION_ARB)
					std::cout << "Error creating context: ERROR_INVALID_VERSION_ARB. " <<
								 "Requested: " << attrib_list[1] << "." << attrib_list[3] << std::endl;
				else if (lastError == ERROR_INVALID_PROFILE_ARB)
					std::cout << "Error creating context: ERROR_INVALID_PROFILE_ARB" << std::endl;
			}
		}

		if (!wglMakeContextCurrentARB(primaryContext.hDC, primaryContext.hDC, primaryContext.hGLRC))
		{
			primaryContext.release();
			return 0;
		}
	}

	glgInit();
	glgInitExts();

	validateExtensions();

	checkOpenGLError("RenderContextPrivate::initOpenGL");

	if (wglSwapIntervalEXT) 
	{
		wglSwapIntervalEXT(params.verticalSync ? 1 : 0);
		checkOpenGLError("RenderContextPrivate::initOpenGL -> wglSwapIntervalEXT");
	}

	return true;
}

int RenderContextPrivate::chooseMSAAPixelFormat(HDC aDC, PIXELFORMATDESCRIPTOR*)
{
	int attributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,     24,
		WGL_ALPHA_BITS_ARB,     8,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB,        0,
		0, 0
	};

	int returnedPixelFormat = 0;
	UINT numFormats = 0;
	BOOL bStatus = FALSE;

	for (int samples = 32; samples > 0; samples /= 2)
	{
		attributes[17] = samples;
		bStatus = wglChoosePixelFormatARB(aDC, attributes, 0, 1, &returnedPixelFormat, &numFormats);
		if (bStatus && numFormats) break;
	}

	return returnedPixelFormat;
}

int RenderContextPrivate::chooseAAPixelFormat(HDC aDC, PIXELFORMATDESCRIPTOR* pfd)
{
	return chooseMSAAPixelFormat(aDC, pfd);
}

int RenderContextPrivate::choosePixelFormat(HDC aDC, PIXELFORMATDESCRIPTOR* pfd)
{
	return ChoosePixelFormat(aDC, pfd);
}

RenderContextPrivate::~RenderContextPrivate()
{
	primaryContext.release();
	UnregisterClassA(wndClass.lpszClassName, hInstance);
}

void RenderContextPrivate::setActive(bool active)
{
	_renderContext->setActive(active);
}

void RenderContextPrivate::resized(const vec2i& size)
{
	_renderContext->resized(size);
}

bool RenderContextPrivate::shouldPostMovementMessage(int x, int y)
{
	if ((x == _mouseX) && (y == _mouseY)) return false;

	_mouseX = x;
	_mouseY = y;
	return true;
}

/*
* Render Context Data
*/ 

RenderContextData& RenderContextData::release()
{
	if (hGLRC != 0)
	{
		if ((hDC != 0) && (wglGetCurrentContext() == hGLRC))
			wglMakeCurrent(hDC, 0);

		wglDeleteContext(hGLRC);
		hGLRC = 0;
	}

	if ((hWnd != 0) && (hDC != 0))
	{
		ReleaseDC(hWnd, hDC);
		hDC = 0;
	}

	if (hWnd != 0)
	{
		DestroyWindow(hWnd);
		hWnd = 0;
	}

	initialized = false;
	return *this;
}

/*
* Primary window proc
*/

union internal_PointerInputInfo
{
	LPARAM lParam;
	struct { short x, y; };

	internal_PointerInputInfo(LPARAM p) : lParam(p) { }
};

union internal_KeyInputInfo
{
	WPARAM wParam;
	struct { char code, unused1, unused2, unused3; };

	internal_KeyInputInfo(WPARAM p) : wParam(p) { }
};

union internal_SizeInfo
{
	LPARAM lParam;
	struct { short width, height; };

	internal_SizeInfo(LPARAM p) : lParam(p) { };
};

static int mouseCaptures = 0;

void captureMouse(HWND wnd)
{
	++mouseCaptures;
	SetCapture(wnd);
}

void releaseMouse()
{
	--mouseCaptures;
	if (mouseCaptures <= 0)
	{
		mouseCaptures = 0;
		ReleaseCapture();
	}
}

LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if ET_PLATFORM_WIN64
	RenderContextPrivate* handler = (RenderContextPrivate*)GetWindowLong(hWnd, GWLP_USERDATA);
#else
	RenderContextPrivate* handler = (RenderContextPrivate*)GetWindowLong(hWnd, GWL_USERDATA);
#endif

	if (handler == 0)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	vec2 viewportSize = handler->renderContext()->size();

	switch (uMsg)
	{ 
	case WM_QUIT:
	case WM_CLOSE:
		{
			application().quit();
			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			captureMouse(hWnd);
			internal_PointerInputInfo p(lParam);

			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);

			handler->pointerPressed(PointerInputInfo(PointerType_General, pt, normPt, 0, 
				PointerType_General, mainRunLoop()->time()));
			return 0;
		}

	case WM_LBUTTONUP:
		{
			releaseMouse();
			internal_PointerInputInfo p(lParam);

			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);

			handler->pointerReleased(PointerInputInfo(PointerType_General, pt, normPt, 0, 
				PointerType_General, mainRunLoop()->time()));
			return 0;
		}

	case WM_RBUTTONDOWN:
		{
			captureMouse(hWnd);
			internal_PointerInputInfo p(lParam);

			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);

			handler->pointerPressed(PointerInputInfo(PointerType_RightButton, pt, normPt, 0, 
				PointerType_RightButton, mainRunLoop()->time()));

			return 0;
		}

	case WM_RBUTTONUP:
		{
			releaseMouse();
			internal_PointerInputInfo p(lParam);
			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);
			handler->pointerReleased(PointerInputInfo(PointerType_RightButton, pt, normPt, 0, 
				PointerType_RightButton, mainRunLoop()->time()));
			return 0;
		}

	case WM_MBUTTONDOWN:
		{
			captureMouse(hWnd);
			internal_PointerInputInfo p(lParam);
			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);
			handler->pointerPressed(PointerInputInfo(PointerType_MiddleButton, pt, normPt, 0, 
				PointerType_MiddleButton, mainRunLoop()->time()));
			return 0;
		}

	case WM_MBUTTONUP:
		{
			releaseMouse();
			internal_PointerInputInfo p(lParam);
			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);
			handler->pointerReleased(PointerInputInfo(PointerType_MiddleButton, pt, normPt, 0,
				PointerType_MiddleButton, mainRunLoop()->time()));
			return 0;
		}

	case WM_KEYDOWN:
		{
			internal_KeyInputInfo k(wParam);
			handler->keyPressed( k.code );
			return 0;
		}

	case WM_KEYUP:
		{
			internal_KeyInputInfo k(wParam);
			handler->keyReleased( k.code );
			return 0;
		}

	case WM_CHAR:
		{
			internal_KeyInputInfo k(wParam);
			handler->charEntered(k.code);
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			internal_PointerInputInfo p(lParam);
			if (handler->shouldPostMovementMessage(p.x, p.y))
			{
				float t = mainRunLoop()->time();
				vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
				vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);

				PointerType pointer = PointerType_None;
				pointer += (wParam & MK_LBUTTON) == MK_LBUTTON ? PointerType_General : 0;
				pointer += (wParam & MK_RBUTTON) == MK_RBUTTON ? PointerType_RightButton : 0;
				pointer += (wParam & MK_MBUTTON) == MK_MBUTTON ? PointerType_MiddleButton : 0;

				handler->pointerMoved(PointerInputInfo(pointer, pt, normPt, 0, pointer, t));
			}
			return 0;
		}

	case WM_MOUSEWHEEL:
		{
			internal_PointerInputInfo p(lParam);
			short s = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

			vec2 pt(static_cast<float>(p.x), static_cast<float>(p.y));
			vec2 normPt(2.0f * pt.x / viewportSize.x - 1.0f, 1.0f - 2.0f * pt.y / viewportSize.y);

			handler->pointerScrolled(PointerInputInfo(PointerType_MiddleButton, pt, normPt, s,
				PointerType_MiddleButton, mainRunLoop()->time()));

			return 0;
		}

	case WM_ACTIVATE:
		{
			handler->setActive(wParam != WA_INACTIVE);
			return 0;
		}

	case WM_SIZE:
		{
			internal_SizeInfo size(lParam);
			handler->resized(vec2i(size.width, size.height));
			return 0;
		}

	default: 
		return DefWindowProc(hWnd, uMsg, wParam, lParam); 
	}
}