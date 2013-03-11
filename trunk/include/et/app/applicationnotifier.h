/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/app/application.h>

namespace et
{
	class ApplicationNotifier
	{
	public:
		void notifyLoaded()
			{ application().loaded(); }
		
		void notifyIdle()
			{ application().idle(); }
		
		void notifyDeactivated()
			{ application().setActive(false); }
		
		void notifyActivated()
			{ application().setActive(true); }
		
		void notifyResize(const et::vec2i& sz)
			{ application().renderContext()->resized(sz); }
		
		RenderContext* accessRenderContext()
			{ return application().renderContext(); }
	};
	
}
