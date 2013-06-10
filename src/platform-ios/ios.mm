/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#import <sys/xattr.h>
#include <Foundation/NSURL.h>
#include <et/platform-ios/ios.h>

using namespace et;

void et::excludeFileFromICloudBackup(const std::string& path)
{
	NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:path.c_str()]];
	
	float version = [[[UIDevice currentDevice] systemVersion] floatValue];
	if (version >= 5.1f)
	{
		NSError* error = nil;
		
		[url setResourceValue:[NSNumber numberWithBool:YES]
			forKey:NSURLIsExcludedFromBackupKey error:&error];
			
		if (error != nil)
			NSLog(@"Failed to exclude file from iCloud backup: %s, error %@", path.c_str(), error);
	}
	else
	{
		u_int8_t attrValue = 1;
		int result = setxattr(path.c_str(), "com.apple.MobileBackup",
			&attrValue, sizeof(attrValue), 0, 0);
		
		if (result != 0)
			NSLog(@"Failed to exclude file from iCloud backup: %s", path.c_str());
	}
}
