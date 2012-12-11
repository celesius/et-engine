//
//  OpenPanel.mm
//  FBXConverter
//
//  Created by Sergey Reznik on 22.10.12.
//  Copyright (c) 2012 Sergey Reznik. All rights reserved.
//

#include "OpenPanel.h"
#import <AppKit/AppKit.h>

std::string fbxc::selectFileToOpen()
{
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setCanChooseFiles:YES];
    [openDlg setCanChooseDirectories:NO];
	[openDlg setAllowsMultipleSelection:NO];
	[openDlg setResolvesAliases:YES];
	[openDlg setAllowedFileTypes:nil];
	
    if ([openDlg runModal] == NSOKButton)
    {
		for (NSURL* url in [openDlg URLs])
		{
			return std::string([[url path] UTF8String]);
		}
    }
	
	return std::string();
}

std::string fbxc::selectFileToSave()
{
	NSSavePanel* saveDlg = [NSSavePanel savePanel];
	
    if ([saveDlg runModal] == NSOKButton)
    {
		return std::string([[[saveDlg URL] path] UTF8String]);
    }
	
	return std::string();
}