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
	
	template <typename T> struct vector2;
	typedef vector2<int> vec2i;
	
	class IApplicationDelegate : virtual public EventReceiver
	{
	public:
		virtual ~IApplicationDelegate() { }

		virtual et::ApplicationIdentifier applicationIdentifier() const = 0;
		
		virtual void setRenderContextParameters(et::RenderContextParameters&) { }
		virtual void applicationDidLoad(et::RenderContext*) { }
		virtual void applicationWillTerminate() { }

		virtual void applicationWillResizeContext(const et::vec2i&) { }

		virtual void applicationWillActivate() { }
		virtual void applicationWillDeactivate() { }

		virtual void render(et::RenderContext*) { }
		virtual void idle(float) { }
	};

}