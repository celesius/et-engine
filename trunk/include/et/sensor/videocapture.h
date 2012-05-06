/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/containers.h>
#include <et/geometry/geometry.h>
#include <et/app/events.h>

namespace et
{
	struct VideoFrameData : public Shared
	{
		vec2i size;
		BinaryDataStorage data;
	};
	typedef IntrusivePtr<VideoFrameData> VideoFrame;
	
	class VideoCapturePrivate;
	class VideoCapture : public EventReceiver
	{
    public:
        static bool available();
        
	public:
		VideoCapture();
		~VideoCapture();
        
        void run();
        void stop();
		
		ET_DECLARE_EVENT1(frameCaptured, VideoFrame);
		
	private:
		friend class VideoCapturePrivate;
		VideoCapturePrivate* _private;
	};
}