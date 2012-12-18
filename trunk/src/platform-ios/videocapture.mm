/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#import <CoreVideo/CoreVideo.h>
#import <AVFoundation/AVFoundation.h>

#include <et/sensor/videocapture.h>

@class VideoCaptureProxy;

namespace et
{
	class VideoCapturePrivate
	{
	public:
		VideoCapturePrivate(VideoCapture* owner);
		~VideoCapturePrivate();
		
		void handleSampleBuffer(CMSampleBufferRef);
        void run();
        void stop();

		void setFocusLocked(bool isLocked);
		void setWhiteBalanceLocked(bool isLocked);
		void setExposureLocked(bool isLocked);
		
	private:
		VideoCapture* _owner;
		VideoCaptureProxy* _proxy;
		AVCaptureSession* _session;
	};
}

using namespace et;

@interface VideoCaptureProxy : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
	VideoCapturePrivate* _p;
}

- (id)initWithVideoCapturePrivate:(VideoCapturePrivate*)p;

- (void)captureOutput:(AVCaptureOutput *)captureOutput
	didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;

@end

@implementation VideoCaptureProxy

- (id)initWithVideoCapturePrivate:(VideoCapturePrivate*)p
{
	self = [super init];
	if (self)
	{
		_p = p;
	}
	return self;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
	   fromConnection:(AVCaptureConnection *)connection
{
	_p->handleSampleBuffer(sampleBuffer);
}

@end

VideoCapture::VideoCapture()
	{ _private = new VideoCapturePrivate(this); }

VideoCapture::~VideoCapture()
	{ delete _private; }

void VideoCapture::run()
    { _private->run(); }

void VideoCapture::stop()
    { _private->stop(); }

void VideoCapture::setFlags(size_t flags)
{
	if ((flags & VideoCaptureFlag_LockFocus) == VideoCaptureFlag_LockFocus)
		_private->setFocusLocked(true);
	if ((flags & VideoCaptureFlag_LockExposure) == VideoCaptureFlag_LockExposure)
		_private->setExposureLocked(true);
	if ((flags & VideoCaptureFlag_LockWhiteBalance) == VideoCaptureFlag_LockWhiteBalance)
		_private->setWhiteBalanceLocked(true);
}

void VideoCapture::removeFlags(size_t flags)
{
	if ((flags & VideoCaptureFlag_LockFocus) == VideoCaptureFlag_LockFocus)
		_private->setFocusLocked(false);
	if ((flags & VideoCaptureFlag_LockExposure) == VideoCaptureFlag_LockExposure)
		_private->setExposureLocked(false);
	if ((flags & VideoCaptureFlag_LockWhiteBalance) == VideoCaptureFlag_LockWhiteBalance)
		_private->setWhiteBalanceLocked(false);
}

bool VideoCapture::available()
{
    return [[AVCaptureDevice devices] count] > 0;
}

VideoCapturePrivate::VideoCapturePrivate(VideoCapture* owner) : _owner(owner)
{
	NSArray* devices = [AVCaptureDevice devices];
    if ([devices count] == 0) return;
    
	_proxy = [[VideoCaptureProxy alloc] initWithVideoCapturePrivate:this];

	_session = [[AVCaptureSession alloc] init];
	_session.sessionPreset = AVCaptureSessionPresetMedium;

	NSError* error = nil;
	AVCaptureDeviceInput* _input = [AVCaptureDeviceInput deviceInputWithDevice:[devices objectAtIndex:0] error:&error];
	if (_input && [_session canAddInput:_input])
		[_session addInput:_input];
	
	AVCaptureVideoDataOutput* _output = [[AVCaptureVideoDataOutput alloc] init];
	_output.alwaysDiscardsLateVideoFrames = YES;
	_output.videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
							 [NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey, nil];
	
	[_output setSampleBufferDelegate:_proxy queue:dispatch_get_main_queue()];
	[_session addOutput:_output];
	
	AVCaptureConnection *conn = [_output connectionWithMediaType:AVMediaTypeVideo];
	if (conn.supportsVideoMinFrameDuration)
		conn.videoMinFrameDuration = CMTimeMake(1, 30);
	
	if (conn.supportsVideoMaxFrameDuration)
		conn.videoMaxFrameDuration = CMTimeMake(1, 15);
	
	[_session startRunning];
}

VideoCapturePrivate::~VideoCapturePrivate()
{
    stop();
    
	[_session release];
	[_proxy release];
}

void VideoCapturePrivate::handleSampleBuffer(CMSampleBufferRef sampleBuffer)
{
	CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
	assert(CVPixelBufferLockBaseAddress(imageBuffer, 0) == kCVReturnSuccess);
	
	VideoFrameData data;
	data.dimensions = vec2i(CVPixelBufferGetWidth(imageBuffer), CVPixelBufferGetHeight(imageBuffer));
	data.data = static_cast<char*>(CVPixelBufferGetBaseAddress(imageBuffer));
	data.dataSize = CVPixelBufferGetDataSize(imageBuffer);
	data.rowSize = CVPixelBufferGetBytesPerRow(imageBuffer);
	_owner->frameDataAvailable.invoke(data);
	
	CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

void VideoCapturePrivate::run()
{
	if (!_session.running)
		[_session startRunning];
}

void VideoCapturePrivate::stop()
{
	if (_session.running)
		[_session stopRunning];
}

void VideoCapturePrivate::setFocusLocked(bool isLocked)
{
	if (_session == nullptr) return;
	
	[_session beginConfiguration];
	
	NSArray* devices = [AVCaptureDevice devices];
	NSError* error = nil;
	
	for (AVCaptureDevice* device in devices)
	{
		if (([device hasMediaType:AVMediaTypeVideo]) && ([device position] == AVCaptureDevicePositionBack))
		{
			[device lockForConfiguration:&error];
			if (isLocked)
			{
				if ([device isFocusModeSupported:AVCaptureFocusModeLocked])
					device.focusMode = AVCaptureFocusModeLocked;
			}
			else
			{
				if ([device isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus])
					device.focusMode = AVCaptureFocusModeContinuousAutoFocus;
			}
			[device unlockForConfiguration];
		}
	}
	
	[_session commitConfiguration];
}

void VideoCapturePrivate::setWhiteBalanceLocked(bool isLocked)
{
	if (_session == nullptr) return;
	
	[_session beginConfiguration];
	
	NSArray* devices = [AVCaptureDevice devices];
	NSError* error = nil;
	
	for (AVCaptureDevice* device in devices)
	{
		if (([device hasMediaType:AVMediaTypeVideo]) && ([device position] == AVCaptureDevicePositionBack))
		{
			[device lockForConfiguration:&error];
			if (isLocked)
			{
				if ([device isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeLocked])
					device.whiteBalanceMode = AVCaptureWhiteBalanceModeLocked;
			}
			else
			{
				if ([device isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance])
					device.whiteBalanceMode = AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance;
			}
			[device unlockForConfiguration];
		}
	}
	
	[_session commitConfiguration];
}

void VideoCapturePrivate::setExposureLocked(bool isLocked)
{
	if (_session == nullptr) return;
	
	[_session beginConfiguration];
	
	NSArray* devices = [AVCaptureDevice devices];
	NSError* error = nil;
	
	for (AVCaptureDevice* device in devices)
	{
		if (([device hasMediaType:AVMediaTypeVideo]) && ([device position] == AVCaptureDevicePositionBack))
		{
			[device lockForConfiguration:&error];
			if (isLocked)
			{
				if ([device isExposureModeSupported:AVCaptureExposureModeLocked])
					device.exposureMode = AVCaptureExposureModeLocked;
			}
			else
			{
				if ([device isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure])
					device.exposureMode = AVCaptureExposureModeContinuousAutoExposure;
			}
			[device unlockForConfiguration];
		}
	}
	
	[_session commitConfiguration];
}
