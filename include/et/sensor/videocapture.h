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
	struct VideoFrameData
	{
		vec2i dimensions;
		
		char* data;
		size_t dataSize;
		size_t rowSize;
		
	public:
		VideoFrameData() : data(0), dataSize(0)
			{ }
	};
	
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
		
		void setFocusLocked(bool isLocked);
		void setWhitebalancedLocked(bool isLocked);
		void setExposureLocked(bool isLocked);
		void setAllParametersLocked(bool isLocked);
		
		ET_DECLARE_EVENT1(frameDataAvailable, VideoFrameData);
		
	private:
		friend class VideoCapturePrivate;
		VideoCapturePrivate* _private;
	};
}