/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <MessageUI/MessageUI.h>
#include <et/app/application.h>
#include <et/platform-ios/mailcomposer.h>

using namespace et;

class et::MailComposerPrivate
{
};

/*
 * MailComposer Obj-C Proxy
 */

@interface MailComposerProxy : NSObject<MFMailComposeViewControllerDelegate>
{
	MailComposerPrivate* _private;
}

+ (MailComposerProxy*)sharedInstanceWithPrivatePtr:(MailComposerPrivate*)p;

- (id)initWithPrivatePtr:(MailComposerPrivate*)p;
- (void)setPrivatePtr:(MailComposerPrivate*)p;

@end

@implementation MailComposerProxy

MailComposerProxy* _sharedInstance = nil;

+ (MailComposerProxy*)sharedInstanceWithPrivatePtr:(MailComposerPrivate*)p
{
	if (_sharedInstance == nil)
		_sharedInstance = [[MailComposerProxy alloc] initWithPrivatePtr:p];
	
	[_sharedInstance setPrivatePtr:p];
	return _sharedInstance;
}

- (id)initWithPrivatePtr:(MailComposerPrivate*)p
{
	self = [super init];
	if (self)
	{
		_private = p;
	}
	return self;
}

- (void)setPrivatePtr:(MailComposerPrivate*)p
{
	_private = p;
}

- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error
{
	UIViewController* mainViewController = reinterpret_cast<UIViewController*>(application().renderingContextHandle());
	[mainViewController dismissModalViewControllerAnimated:YES];
}
						   
@end

/*
 * MailComposer
 */

MailComposer::MailComposer() : _private(new MailComposerPrivate())
{
	
}

MailComposer::~MailComposer()
{
	[MailComposerProxy sharedInstanceWithPrivatePtr:0];
	delete _private;
}

void MailComposer::composeEmail(const std::string& recepient, const std::string& title, const std::string& text)
{
	MFMailComposeViewController* viewController = [[MFMailComposeViewController alloc] init];
    if (viewController == nil) return;
    
	UIViewController* mainViewController = reinterpret_cast<UIViewController*>(application().renderingContextHandle());
	[viewController setSubject:[NSString stringWithUTF8String:title.c_str()]];
	[viewController setToRecipients:[NSArray arrayWithObject:[NSString stringWithUTF8String:recepient.c_str()]]];
	[viewController setMessageBody:[NSString stringWithUTF8String:text.c_str()] isHTML:NO];
	[viewController setMailComposeDelegate:[MailComposerProxy sharedInstanceWithPrivatePtr:_private]];
	[mainViewController presentModalViewController:[viewController autorelease] animated:YES];
}

bool MailComposer::canSendEmail() const
{
	return [MFMailComposeViewController canSendMail];
}