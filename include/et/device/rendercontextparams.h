#include <et/geometry/geometry.h>

namespace et
{
	
	enum WindowStyle 
	{
		WindowStyle_FixedWithCaption,
		WindowStyle_FixedWithoutCaption,
		WindowStyle_Sizable,
		WindowStyle_StretchedToWorkarea,
		WindowStyle_Fullscreen
	};
	
	enum MultisamplingQuality
	{
		MultisamplingQuality_None,
		MultisamplingQuality_Best,
		MultisamplingQuality_Max
	};
	
	struct RenderContextParameters
	{
		WindowStyle windowStyle;
		MultisamplingQuality multisamplingQuality;
		bool openGLForwardContext;
		bool openGLCoreProfile;
		bool verticalSync;
		bool supportLandscapeOrientation;
		bool supportPortraitOrientation;
		vec2i contextPosition;
		vec2i contextSize;
		vec2i openGLMaxVersion;
		vec2i baseContextSize;
		
		RenderContextParameters(
#if defined(ET_PLATFORM_IOS)
								WindowStyle ws = WindowStyle_Fullscreen, 
								MultisamplingQuality ms = MultisamplingQuality_None,
								const vec2i& windowPos = vec2i(0, 0), 
								const vec2i& windowSize = vec2i(480, 320),
								const vec2i& openGLMaxVer = vec2i(2, 0), 
								const vec2i& baseScrSize = vec2i(512, 512),
								bool forwardContext = false, 
								bool coreProfile = false,
								bool vSync = true,
								bool landscapeOrientation = true,
								bool portraitOrientation = false
#else 
								WindowStyle ws = WindowStyle_FixedWithCaption, 
								MultisamplingQuality ms = MultisamplingQuality_Best,
								const vec2i& windowPos = vec2i(-1), 
								const vec2i& windowSize = vec2i(800, 600),
								const vec2i& openGLMaxVer = vec2i(4, 2),
								const vec2i& baseScrSize = vec2i(512, 512),
								bool forwardContext = true, 
									bool coreProfile = true,
								bool vSync = false,
								bool landscapeOrientation = true,
								bool portraitOrientation = false
#endif								
								) : 
		windowStyle(ws), 
		multisamplingQuality(ms), 
		openGLForwardContext(forwardContext), 
		openGLCoreProfile(coreProfile), 
		contextPosition(windowPos), 
		contextSize(windowSize), 
		openGLMaxVersion(openGLMaxVer), 
		baseContextSize(baseScrSize),
		verticalSync(vSync),
		supportLandscapeOrientation(landscapeOrientation),
		supportPortraitOrientation(portraitOrientation)
		{
		}
	};
	
}