/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/framebufferdata.h>

namespace et
{
	class Framebuffer : public IntrusivePtr<FramebufferData>
	{
	public:
		Framebuffer() :
			IntrusivePtr<FramebufferData>(nullptr) { };
		
	private:
		friend class FramebufferFactory;
		
		Framebuffer(FramebufferData* data) :
			IntrusivePtr<FramebufferData>(data) { };
	};
}
