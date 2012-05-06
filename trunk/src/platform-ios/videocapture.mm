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

VideoCapturePrivate::VideoCapturePrivate(VideoCapture* owner) : _owner(owner)
{
	_proxy = [[VideoCaptureProxy alloc] initWithVideoCapturePrivate:this];
	
	AVCaptureSession* _session = [[AVCaptureSession alloc] init];
	_session.sessionPreset = AVCaptureSessionPresetHigh;
	
	NSArray* devices = [AVCaptureDevice devices];
	
	NSError* error = nil;
	AVCaptureDeviceInput* _input = [AVCaptureDeviceInput deviceInputWithDevice:[devices objectAtIndex:0] error:&error];
	if (_input)
		[_session addInput:_input];
	
	AVCaptureVideoDataOutput* _output = [[AVCaptureVideoDataOutput alloc] init];
	_output.videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
							 [NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
							 [NSNumber numberWithBool:NO], kCVPixelBufferOpenGLCompatibilityKey, nil];
	
	dispatch_queue_t queue = dispatch_queue_create("dispatch-queue", nil);
	[_output setSampleBufferDelegate:_proxy queue:queue];
	dispatch_release(queue);
	
	[_session addOutput:_output];
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
	
	void* ptr = CVPixelBufferGetBaseAddress(imageBuffer);
	
	VideoFrame f(new VideoFrameData);
	f->size = vec2i(CVPixelBufferGetWidth(imageBuffer), CVPixelBufferGetHeight(imageBuffer));
	f->data = BinaryDataStorage(CVPixelBufferGetDataSize(imageBuffer));
	memcpy(f->data.data(), ptr, f->data.dataSize());
	
	_owner->frameCaptured.invokeInMainRunLoop(f);
	
	CVPixelBufferUnlockBaseAddress(imageBuffer, 0);	
}

void VideoCapturePrivate::run()
{
	[_session startRunning];
}

void VideoCapturePrivate::stop()
{
	[_session stopRunning];
}