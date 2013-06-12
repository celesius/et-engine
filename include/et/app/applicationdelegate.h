/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

namespace et
{
	class RenderContext;
	struct RenderContextParameters;
	template <typename T> struct vector2;
	typedef vector2<int> vec2i;
	
	struct ApplicationIdentifier
	{
		std::string identifier;
		std::string companyName;
		std::string applicationName;
		
		ApplicationIdentifier() { }
		
		ApplicationIdentifier(const std::string& aIdentifier, const std::string& aCompanyName,
			const std::string& aApplicationName) : identifier(aIdentifier), companyName(aCompanyName),
			applicationName(aApplicationName) { }
	};

	enum WindowStyle
	{
		WindowStyle_FixedWithCaption,
		WindowStyle_FixedWithoutCaption,
		WindowStyle_Sizable,
		WindowStyle_StretchedToWorkarea,
		WindowStyle_Fullscreen
	};

	struct ApplicationParameters
	{
		WindowStyle windowStyle;
		bool shouldSuspendOnDeactivate;

		ApplicationParameters(
#if (ET_PLATFORM_IOS || ET_PLATFORM_ANDROID)
							  WindowStyle ws = WindowStyle_Fullscreen
#else
							  WindowStyle ws = WindowStyle_FixedWithCaption
#endif
							  ) : windowStyle(ws) { }
	};
	
	class IApplicationDelegate : virtual public EventReceiver
	{
	public:
		virtual ~IApplicationDelegate() { }

		virtual et::ApplicationIdentifier applicationIdentifier() const = 0;
		
		virtual void setApplicationParameters(et::ApplicationParameters&) { }
		virtual void setRenderContextParameters(et::RenderContextParameters&) { }
		
		virtual void applicationDidLoad(et::RenderContext*) { }
		virtual void applicationWillActivate() { }
		virtual void applicationWillDeactivate() { }
		virtual void applicationWillSuspend() { }
		virtual void applicationWillResume() { }
		virtual void applicationWillTerminate() { }

		virtual void applicationWillResizeContext(const et::vec2i&) { }

		virtual void render(et::RenderContext*) { }
		virtual void idle(float) { }
	};

}