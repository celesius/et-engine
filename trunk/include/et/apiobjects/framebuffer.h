/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/framebufferdata.h>

namespace et
{
	
	class Framebuffer : public IntrusivePtr<FramebufferData>
	{
		friend class FramebufferFactory;
		Framebuffer(FramebufferData* data) : IntrusivePtr<FramebufferData>(data) { };
		
	public:
		Framebuffer() : IntrusivePtr<FramebufferData>(0) { };
		
	};
	
}