/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <AppKit/NSOpenPanel.h>
#include <et/platform/platformtools.h>

using namespace et;

std::string et::selectFile(const StringList&, SelectFileMode mode)
{
	if (mode == SelectFileMode_Open)
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
				return std::string([[url path] UTF8String]);
		}
		
		return std::string();
	}
	else if (mode == SelectFileMode_Save)
	{
		NSSavePanel* saveDlg = [NSSavePanel savePanel];
		
		if ([saveDlg runModal] == NSOKButton)
			return std::string([[[saveDlg URL] path] UTF8String]);
		
		return std::string();
	}
	else
	{
		assert(0 && "Invalid SelectFieMode value");
		return std::string();
	}
}
