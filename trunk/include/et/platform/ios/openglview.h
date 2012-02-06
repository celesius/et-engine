#import <UIKit/UIKit.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import <et/apiobjects/framebuffer.h>
#import <et/input/input.h>

namespace et
{
	class RenderContextNotifier;
}

@interface etOpenGLView : UIView
{
@private
    EAGLContext* _context;
	et::Framebuffer _defaultFramebuffer;
	et::RenderContext* _rc;
	et::RenderContextNotifier* _rcNotifier;
	et::Input::PointerInputSource _inputSource;
}

- (id)initWithFrame:(CGRect)frame;
- (void)setRenderContext:(et::RenderContext*)rc;
- (void)beginRender;
- (void)endRender;

@property (nonatomic, retain) EAGLContext* context;

- (const et::Framebuffer&)defaultFramebuffer;


@end
