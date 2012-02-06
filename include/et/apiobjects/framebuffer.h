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