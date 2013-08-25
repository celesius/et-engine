/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <Foundation/NSInvocation.h>
#include <AppKit/NSOpenPanel.h>
#include <et/platform/platformtools.h>

using namespace et;

@interface FilePicker : NSObject

+ (FilePicker*)picker;
- (NSString*)openFile;
- (NSString*)saveFile;

@end

std::string et::selectFile(const StringList&, SelectFileMode mode)
{
	NSString* pickerResult = nil;
	SEL selector = nil;

	if (mode == SelectFileMode_Open)
		selector = @selector(openFile);
	else if (mode == SelectFileMode_Save)
		selector = @selector(saveFile);
	else
		assert("Invalid SelectFieMode value" && 0);

	NSInvocation* i = [NSInvocation invocationWithMethodSignature:
		[FilePicker instanceMethodSignatureForSelector:selector]];

	[i setSelector:selector];

	[i performSelectorOnMainThread:@selector(invokeWithTarget:)
		withObject:[FilePicker picker] waitUntilDone:YES];
	
	[i getReturnValue:&pickerResult];

#if (ET_OBJC_ARC_ENABLED)
	return std::string([pickerResult UTF8String]);
#else
	return std::string([[pickerResult autorelease] UTF8String]);
#endif
}

@implementation FilePicker

+ (FilePicker*)picker
{
#if (ET_OBJC_ARC_ENABLED)
	return [[FilePicker alloc] init];
#else
	return [[[FilePicker alloc] init] autorelease];
#endif
}

- (NSString*)openFile
{
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	[openDlg setCanChooseFiles:YES];
	[openDlg setCanChooseDirectories:NO];
	[openDlg setAllowsMultipleSelection:NO];
	[openDlg setResolvesAliases:YES];
	[openDlg setAllowedFileTypes:nil];

#if (ET_OBJC_ARC_ENABLED)
	return ([openDlg runModal] == NSOKButton) ?
		[[[openDlg URLs] objectAtIndex:0] path] : [[NSString alloc] init];
#else
	return ([openDlg runModal] == NSOKButton) ?
		[[[[openDlg URLs] objectAtIndex:0] path] retain] : [[NSString alloc] init];
#endif
}

- (NSString*)saveFile
{
	NSSavePanel* saveDlg = [NSSavePanel savePanel];
#if (ET_OBJC_ARC_ENABLED)
	return ([saveDlg runModal] == NSOKButton) ? [[saveDlg URL] path] : [[NSString alloc] init];
#else
	return ([saveDlg runModal] == NSOKButton) ? [[[saveDlg URL] path] retain] : [[NSString alloc] init];
#endif
}

@end