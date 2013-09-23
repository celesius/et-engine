/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <AppKit/NSWindow.h>
#include <AppKit/NSAlert.h>
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
{
	NSMutableCharacterSet* allowedCharacters;
}

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag;

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
	void performUpdateAndRender();
	void stop();
		
	bool canPerformOperations()
		{ return !firstSync && (displayLink != nil); }
	
private:
	etWindowDelegate* windowDelegate = nil;
	etOpenGLView* openGlView = nil;
	etOpenGLWindow* mainWindow = nil;
	
	NSOpenGLPixelFormat* pixelFormat = nil;
	NSOpenGLContext* openGlContext = nil;
	CVDisplayLinkRef displayLink = nullptr;
	CGLContextObj cglObject = nullptr;
	
	bool firstSync = true;
	bool resizeScheduled = false;
	NSSize scheduledSize = { };
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
	_renderer.release();
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
	CVOptionFlags, CVOptionFlags*, void* displayLinkContext);

RenderContextPrivate::RenderContextPrivate(RenderContext*, RenderContextParameters& params,
	const ApplicationParameters& appParams)
{
	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
	{
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 32,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFASampleBuffers, 4,
		0, 0, 0, 0, 0, 0, // space for multisampling and context profile
		0,
	};
	
	size_t lastEntry = 0;
	while (pixelFormatAttributes[++lastEntry] != 0);
	
	size_t profileEntry = 0;
	if (params.openGLForwardContext)
	{
		pixelFormatAttributes[lastEntry++] = NSOpenGLPFAOpenGLProfile;
		
		pixelFormatAttributes[lastEntry++] =
			params.openGLCoreProfile ? NSOpenGLProfileVersion3_2Core : NSOpenGLProfileVersionLegacy;
		
		profileEntry = lastEntry - 1;
	}
	
	size_t antialiasFirstEntry = 0;
	size_t antialiasSamplesEntry = 0;
	if (params.multisamplingQuality == MultisamplingQuality_Best)
	{
		antialiasFirstEntry = lastEntry;
		pixelFormatAttributes[lastEntry++] = NSOpenGLPFAMultisample;
		pixelFormatAttributes[lastEntry++] = NSOpenGLPFASamples;
		pixelFormatAttributes[lastEntry++] = 16;
		antialiasSamplesEntry = lastEntry - 1;
	}
	
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
	if ((pixelFormat == nil) && (antialiasSamplesEntry != 0))
	{
		while ((pixelFormat == nil) && (pixelFormatAttributes[antialiasSamplesEntry] > 1))
		{
			pixelFormatAttributes[antialiasSamplesEntry] /= 2;
			pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
		}
		
		if (pixelFormat == nil)
		{
			pixelFormatAttributes[antialiasFirstEntry++] = 0;
			pixelFormatAttributes[antialiasFirstEntry++] = 0;
			pixelFormatAttributes[antialiasFirstEntry++] = 0;
			pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
		}
		
		if (pixelFormat == nil)
		{
			if (profileEntry > 0)
			{
				pixelFormatAttributes[profileEntry] = NSOpenGLProfileVersionLegacy;
				pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
				
				if (pixelFormat == nil)
				{
					[[NSAlert alertWithMessageText:@"Unable to init OpenGL context" defaultButton:@"Close"
						alternateButton:nil otherButton:nil informativeTextWithFormat:
						@"Unable to create NSOpenGLPixelFormat object, even without antialiasing and with legacy profile."] runModal];
					exit(1);
				}
			}
			else
			{
				[[NSAlert alertWithMessageText:@"Unable to init OpenGL context" defaultButton:@"Close"
					alternateButton:nil otherButton:nil informativeTextWithFormat:
					@"Unable to create NSOpenGLPixelFormat object, even without antialiasing."] runModal];
				exit(1);
			}
		}
	}
	
#if (!ET_OBJC_ARC_ENABLED)
	[pixelFormat autorelease];
#endif
	
	NSUInteger windowMask = NSBorderlessWindowMask;
	
	if (appParams.windowSize != WindowSize_Fullscreen)
	{
		if (appParams.windowStyle & WindowStyle_Caption)
			windowMask |= NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
		
		if (appParams.windowStyle & WindowStyle_Sizable)
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
	
	mainWindow = [[etOpenGLWindow alloc] initWithContentRect:contentRect
		styleMask:windowMask backing:NSBackingStoreBuffered defer:YES];
	
	if (appParams.keepWindowAspectOnResize)
		[mainWindow setContentAspectRatio:contentRect.size];

#if (ET_OBJC_ARC_ENABLED)
	CFRetain((__bridge CFTypeRef)mainWindow);
#endif
		
	params.contextSize = vec2i(static_cast<int>(contentRect.size.width),
		static_cast<int>(contentRect.size.height));
	
	windowDelegate = [[etWindowDelegate alloc] init];
	windowDelegate->rcPrivate = this;
	[mainWindow setDelegate:windowDelegate];
	[mainWindow setOpaque:YES];
	
	openGlView = [[etOpenGLView alloc] initWithFrame:NSMakeRect(0.0f, 0.0f, contentRect.size.width,
		contentRect.size.height) pixelFormat:pixelFormat];
	openGlView->rcPrivate = this;
	[openGlView setWantsBestResolutionOpenGLSurface:YES];
	
	openGlContext = [openGlView openGLContext];
	[openGlContext makeCurrentContext];
	
#if (!ET_OBJC_ARC_ENABLED)
	[openGlContext retain];
#endif
	
	cglObject = static_cast<CGLContextObj>([openGlContext CGLContextObj]);
	
	const int swap = 1;
	CGLSetParameter(cglObject, kCGLCPSwapInterval, &swap);
	
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
#if (!ET_OBJC_ARC_ENABLED)
	[openGlView release];
	[openGlContext release];
	[windowDelegate release];
#endif
	openGlView = nil;
	openGlContext = nil;
	mainWindow = nil;
	windowDelegate = nil;
}

void RenderContextPrivate::run()
{
	if (displayLink == nil)
	{
		CVReturn result = CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
		
		if ((result != kCVReturnSuccess) || (displayLink == nullptr))
		{
			[[NSAlert alertWithMessageText:@"Something went wrong, could not create display link."
				defaultButton:@"Ok" alternateButton:nil otherButton:nil
				informativeTextWithFormat:@"Return code: %d, Application will now shut down.", result] runModal];
			exit(1);
			return;
		}
		
		CVDisplayLinkSetOutputCallback(displayLink, cvDisplayLinkOutputCallback, this);
		
		CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglObject,
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

void RenderContextPrivate::performUpdateAndRender()
{
	[openGlContext makeCurrentContext];
	CGLLockContext(cglObject);
	
	windowDelegate->applicationNotifier.notifyIdle();
	
	CGLFlushDrawable(cglObject);
	CGLUnlockContext(cglObject);
}

int RenderContextPrivate::displayLinkSynchronized()
{
	if (firstSync)
	{
		ThreadId currentThread = Threading::currentThread();
		Threading::setMainThread(currentThread);
		Threading::setRenderingThread(currentThread);
		firstSync = false;
		
		if (resizeScheduled)
			resize(scheduledSize);
	}
	
	if (application().running() && !application().suspended())
		performUpdateAndRender();

	return kCVReturnSuccess;
}

void RenderContextPrivate::resize(const NSSize& sz)
{
	if (canPerformOperations())
	{
		[openGlContext makeCurrentContext];
		CGLLockContext(cglObject);
		windowDelegate->applicationNotifier.notifyResize(vec2i(static_cast<int>(sz.width),
			static_cast<int>(sz.height)));
		CGLUnlockContext(cglObject);
		resizeScheduled = false;
	}
	else
	{
		scheduledSize = sz;
		resizeScheduled = true;
	}
}

/*
 * Display link callback
 */
CVReturn cvDisplayLinkOutputCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*,
	CVOptionFlags, CVOptionFlags*, void* displayLinkContext)
{
	@autoreleasepool
	{
		return static_cast<RenderContextPrivate*>(displayLinkContext)->displayLinkSynchronized();
	}
}

/*
 * OpenGL View implementation
 */
@implementation etOpenGLView

- (PointerInputInfo)mousePointerInfo:(NSEvent*)theEvent withType:(PointerType)type;
{
	NSRect ownFrame = self.frame;
	
	NSPoint nativePoint = [theEvent locationInWindow];
	
	vec2 p(static_cast<float>(nativePoint.x),
		static_cast<float>(ownFrame.size.height - nativePoint.y));
	
	vec2 np(2.0f * p.x / static_cast<float>(ownFrame.size.width) - 1.0f,
		1.0f - 2.0f * p.y / static_cast<float>(ownFrame.size.height));

	return PointerInputInfo(type, p, np, vec2(0.0f), static_cast<size_t>([theEvent eventNumber]),
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

	vec2 p(static_cast<float>(nativePoint.x),
		static_cast<float>(ownFrame.size.height - nativePoint.y));
	
	vec2 np(2.0f * p.x / static_cast<float>(ownFrame.size.width) - 1.0f,
		1.0f - 2.0f * p.y / static_cast<float>(ownFrame.size.height));
	
	vec2 scroll(static_cast<float>([theEvent deltaX] / ownFrame.size.width),
		static_cast<float>([theEvent deltaY] / ownFrame.size.height));

	PointerOrigin origin = (([theEvent momentumPhase] != NSEventPhaseNone) ||
		([theEvent phase] != NSEventPhaseNone)) ? PointerOrigin_Trackpad : PointerOrigin_Mouse;

	pointerInputSource.pointerScrolled(PointerInputInfo(PointerType_General, p, np,
		scroll, [theEvent hash], static_cast<float>([theEvent timestamp]), origin));
}

- (void)magnifyWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(
		GestureInputInfo(GestureTypeMask_Zoom, static_cast<float>(event.magnification)));
}

- (void)swipeWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(GestureInputInfo(GestureTypeMask_Swipe,
		static_cast<float>(event.deltaX), static_cast<float>(event.deltaY)));
}

- (void)rotateWithEvent:(NSEvent *)event
{
	gestureInputSource.gesturePerformed(
		GestureInputInfo(GestureTypeMask_Rotate, event.rotation));
}

- (void)drawRect:(NSRect)dirtyRect
{
	(void)dirtyRect;
	
	if (rcPrivate->canPerformOperations())
		rcPrivate->performUpdateAndRender();
}

- (void)reshape
{
	[super reshape];
	
	if (_trackingArea)
		[self removeTrackingArea:_trackingArea];
	
	_trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
		options:NSTrackingMouseMoved | NSTrackingActiveAlways owner:self userInfo:nil];

#if (!ET_OBJC_ARC_ENABLED)
	[_trackingArea autorelease];
#endif
	
	[self addTrackingArea:_trackingArea];
	
	rcPrivate->resize(self.bounds.size);
}

@end

@implementation etOpenGLWindow : NSWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle
	backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
	self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag];
	if (self)
	{
		allowedCharacters = [[NSMutableCharacterSet alloc] init];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet alphanumericCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet punctuationCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet symbolCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet symbolCharacterSet]];
		[allowedCharacters formUnionWithCharacterSet:[NSCharacterSet whitespaceCharacterSet]];
	}
	return self;
}

- (void)keyDown:(NSEvent*)theEvent
{
	Input::KeyboardInputSource().keyPressed(theEvent.keyCode);
	
	NSString* filteredString = [theEvent.characters
		stringByTrimmingCharactersInSet:[allowedCharacters invertedSet]];
	
	if ([filteredString length] > 0)
	{
		std::string cString([filteredString cStringUsingEncoding:NSUTF8StringEncoding]);
		Input::KeyboardInputSource().charactersEntered(cString);
	}
}

- (void)keyUp:(NSEvent*)theEvent
{
	Input::KeyboardInputSource().keyReleased(theEvent.keyCode);
}

@end

@implementation etWindowDelegate

- (void)windowWillClose:(NSNotification *)notification
{
	(void)notification;
	applicationNotifier.notifyStopped();
	rcPrivate->stop();
}

@end
