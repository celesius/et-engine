#import <UIKit/UIKit.h>
#import <et/app/events.h>

namespace et
{
	class ApplicationNotifier;
}

@interface etApplicationDelegate : UIResponder <UIApplicationDelegate> 
{
@private	
	et::ApplicationNotifier* _notifier;
	UIWindow* _window;
	CADisplayLink* _displayLink;
	BOOL _updating;
}

@property (nonatomic, retain) UIWindow* window;

- (void)beginUpdates;
- (BOOL)updating;
- (void)endUpdates;

@end