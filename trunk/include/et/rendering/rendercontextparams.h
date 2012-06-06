/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

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
    
    enum InterfaceOrientation
    {
        InterfaceOrientation_Portrait = 0x01,
        InterfaceOrientation_PortraitUpsideDown = 0x02,
        InterfaceOrientation_LandscapeLeft = 0x04,
        InterfaceOrientation_LandscapeRight = 0x08,
        
        InterfaceOrientation_AnyPortrait = InterfaceOrientation_Portrait + InterfaceOrientation_PortraitUpsideDown,
        InterfaceOrientation_AnyLandscape = InterfaceOrientation_LandscapeLeft + InterfaceOrientation_LandscapeRight,
        InterfaceOrientation_Any = InterfaceOrientation_AnyPortrait + InterfaceOrientation_AnyLandscape
    };
	
	struct RenderContextParameters
	{
		WindowStyle windowStyle;
		MultisamplingQuality multisamplingQuality;
		bool openGLForwardContext;
		bool openGLCoreProfile;
		bool openGLCompatibilityProfile;
		bool verticalSync;
		bool multipleTouch;
        
        size_t supportedInterfaceOrientations;
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
								const vec2i& baseScrSize = vec2i(480, 320),
								bool forwardContext = false, 
								bool coreProfile = false,
								bool compatibilityProfile = false,
								bool vSync = true,
								bool mTouch = true,
                                size_t orientationFlags = InterfaceOrientation_AnyLandscape
#else 
								WindowStyle ws = WindowStyle_FixedWithCaption, 
								MultisamplingQuality ms = MultisamplingQuality_Best,
								const vec2i& windowPos = vec2i(-1), 
								const vec2i& windowSize = vec2i(800, 600),
								const vec2i& openGLMaxVer = vec2i(4, 2),
								const vec2i& baseScrSize = vec2i(512, 512),
								bool forwardContext = true, 
								bool coreProfile = true,
								bool compatibilityProfile = false,
								bool vSync = false,
								bool mTouch = false,
                                size_t orientationFlags = InterfaceOrientation_Any
#endif								
								) : 
		windowStyle(ws), 
		multisamplingQuality(ms), 
		openGLForwardContext(forwardContext), 
		openGLCoreProfile(coreProfile), 
		openGLCompatibilityProfile(compatibilityProfile),
		contextPosition(windowPos), 
		contextSize(windowSize), 
		openGLMaxVersion(openGLMaxVer), 
		baseContextSize(baseScrSize),
		verticalSync(vSync),
		multipleTouch(mTouch),
		supportedInterfaceOrientations(orientationFlags)
		{ }
	};
	
}