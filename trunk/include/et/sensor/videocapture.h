//
//  videocapture.h
//
//  Created by Sergey Reznik on 4/12/12.
//  Copyright (c) 2012 Cheetek. All rights reserved.
//

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