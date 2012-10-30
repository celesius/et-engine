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
		
	private:
		VideoCapture* _owner;
		VideoCaptureProxy* _proxy;
		AVCaptureSession* _session;
	};
}

using namespace et;

@interface VideoCaptureProxy : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
	{ VideoCapturePrivate* _p; }
- (id)initWithVideoCapturePrivate:(VideoCapturePrivate*)p;
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;
@end

@implementation VideoCaptureProxy
- (id)initWithVideoCapturePrivate:(VideoCapturePrivate*)p {
	self = [super init];
	if (self) { _p = p; }
	return self;
}
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
	{ _p->handleSampleBuffer(sampleBuffer); }
@end

VideoCapture::VideoCapture()
	{ _private = new VideoCapturePrivate(this); }

VideoCapture::~VideoCapture()
	{ delete _private; }

void VideoCapture::run()
    { _private->run(); }

void VideoCapture::stop()
    { _private->stop(); }

bool VideoCapture::available()
{
    return [[AVCaptureDevice devices] count] > 0;
}

VideoCapturePrivate::VideoCapturePrivate(VideoCapture* owner) : _owner(owner)
{
	NSArray* devices = [AVCaptureDevice devices];
    if ([devices count] == 0) return;
    
	_proxy = [[VideoCaptureProxy alloc] initWithVideoCapturePrivate:this];

	AVCaptureSession* _session = [[AVCaptureSession alloc] init];
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
		conn.videoMinFrameDuration = CMTimeMake(1, 60);
	
	if (conn.supportsVideoMaxFrameDuration)
		conn.videoMaxFrameDuration = CMTimeMake(1, 1);
	
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