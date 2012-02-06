#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <et/device/rendercontextparams.h>
#import <et/platform/ios/openglview.h>

@interface etOpenGLViewController : UIViewController
{
	EAGLContext* _context;
	etOpenGLView* _glView;
	et::RenderContextParameters _params;
}

- (id)initWithParameters:(et::RenderContextParameters)params;
- (void)setRenderContext:(et::RenderContext*)rc;

- (void)beginRender;
- (void)endRender;

@end
