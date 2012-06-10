/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#include <fstream>
#include <et/core/tools.h>
#include <et/core/containers.h>
#include <et/locale/locale.h>

using namespace et;

std::string Locale::time()
{
	NSDateFormatter* formatter = [[[NSDateFormatter alloc] init] autorelease];
	[formatter setTimeStyle:kCFDateFormatterMediumStyle];
	[formatter setDateStyle:kCFDateFormatterNoStyle];
	[formatter setLocale:[NSLocale currentLocale]];
	return std::string([[formatter stringFromDate:[NSDate date]] cStringUsingEncoding:NSUTF8StringEncoding]);
}

std::string Locale::date()
{
	NSDateFormatter* formatter = [[[NSDateFormatter alloc] init] autorelease];
	[formatter setTimeStyle:kCFDateFormatterNoStyle];
	[formatter setDateStyle:kCFDateFormatterMediumStyle];
	[formatter setLocale:[NSLocale currentLocale]];
	return std::string([[formatter stringFromDate:[NSDate date]] cStringUsingEncoding:NSUTF8StringEncoding]);
}

size_t Locale::currentLocale()
{
    NSString* localeId = [[NSLocale currentLocale] localeIdentifier];
	std::string mbcs = [localeId cStringUsingEncoding:NSUTF8StringEncoding];
    
	size_t result = 0;
    
	if ((mbcs.size() == 5) && (mbcs[2] == '_'))
	{
		lowercase(mbcs);
		result = mbcs[0] | (mbcs[1] << 8) | (mbcs[3] << 16) | (mbcs[4] << 24);
	}

	return result;
}
