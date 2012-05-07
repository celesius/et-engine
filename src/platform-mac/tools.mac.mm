/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Foundation/Foundation.h>

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <et/core/tools.h>

using namespace std;

static double startTime = 0.0;
static bool startTimeInitialized = false;

char et::pathDelimiter = '/';

double __queryTime()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1000000.0;
}

float et::queryTime()
{
	if (!startTimeInitialized)
	{
		startTime = __queryTime();
		startTimeInitialized = true;
	};
	
	return static_cast<float>(__queryTime() - startTime);
} 

std::string et::normalizeFilePath(string s)
{
	for (string::iterator i = s.begin(), e = s.end(); i != e; ++i)
	{
		if ((*i) == '\\')
			(*i) = pathDelimiter;
	}
	return s;
}

bool et::fileExists(const std::string& name)
{
	return access(name.c_str(), R_OK) == 0; 
}

void et::findFiles(const std::string& aFolder, const std::string& mask, bool /* recursive */, std::vector<std::string>& list)
{
	std::string folder = addTrailingSlash(aFolder);
	
	size_t maskLength = mask.length();
	size_t nameSearchCriteria = mask.find_last_of(".*");
	bool searchByName = nameSearchCriteria == maskLength - 1;
	bool searchByExt = mask.find_first_of("*.") == 0;
	
	@autoreleasepool 
	{
		NSError* err = nil;
		NSString* path = [NSString stringWithCString:folder.c_str() encoding:NSASCIIStringEncoding];
		NSArray* files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:&err];
		
		NSArray* filtered = files;
		
		if (searchByExt)
		{
			std::string extMask = mask.substr(2, mask.length() - 2);
			NSString* objcMask = [NSString stringWithCString:extMask.c_str() encoding:NSASCIIStringEncoding];
			filtered = [filtered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH %@", objcMask]];
		}
		
		if (searchByName)
		{
			std::string nameMask = mask.substr(0, mask.length() - 3);
			NSString* objcMask = [NSString stringWithCString:nameMask.c_str() encoding:NSASCIIStringEncoding];
			objcMask = [objcMask stringByAppendingString:@"*"];
			filtered = [filtered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self LIKE %@", objcMask]];
		}
		
		for (NSString* file in filtered)
			list.push_back(folder + std::string([file cStringUsingEncoding:NSASCIIStringEncoding]));
	}
}