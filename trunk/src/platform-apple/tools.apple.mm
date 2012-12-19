/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

#include <sys/time.h>
#include <et/platform/platform.h>
#include <et/core/tools.h>
#include <et/core/containers.h>

#if (ET_PLATFORM_MAC)
#	include <AppKit/AppKit.h>
#endif

using namespace std;

static double startTime = 0.0;
static bool startTimeInitialized = false;

char et::pathDelimiter = '/';

double __queryTime();

float et::queryTime()
{
	if (!startTimeInitialized)
	{
		startTime = __queryTime();
		startTimeInitialized = true;
	};
	
	return static_cast<float>(__queryTime() - startTime);
}

double __queryTime()
{
	timeval tv = { };
	gettimeofday(&tv, 0);
	return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1000000.0;
}

std::string et::applicationPath()
{
    char result[256] = { };
    
    CFURLRef bUrl = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFURLGetFileSystemRepresentation(bUrl, true, reinterpret_cast<UInt8*>(result), 256);
    CFRelease(bUrl);
    
    size_t i = 0;
    while ((i < 256) && result[++i]) { };
    result[i] = '/';
    
    return std::string(result);
}

std::string et::applicationDataFolder()
{
    char result[256] = { };
    
	CFURLRef bUrl = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    CFURLGetFileSystemRepresentation(bUrl, true, reinterpret_cast<UInt8*>(result), 256);
    CFRelease(bUrl);
    
    size_t i = 0;
    while ((i < 256) && result[++i]) { };
    result[i] = '/';
    
	return std::string(result);
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
    NSString* fileName = [[NSString alloc] initWithUTF8String:name.c_str()];
	
	BOOL isDir = NO;
    BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:fileName isDirectory:&isDir];
    [fileName release];
	
    return exists && !isDir;
}

bool et::folderExists(const std::string& name)
{
    NSString* fileName = [[NSString alloc] initWithUTF8String:name.c_str()];
	
	BOOL isDir = NO;
	BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:fileName isDirectory:&isDir];
    [fileName release];
	
	return exists && isDir;
}

std::string et::applicationLibraryBaseFolder()
{
    @autoreleasepool
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
        NSString* folder = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
        return std::string([folder cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

std::string et::applicationDocumentsBaseFolder()
{
    @autoreleasepool 
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* folder = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
        return std::string([folder cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

void et::createDirectory(const std::string& name)
{
	NSError* err = nil;
	NSString* path = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
	[[NSFileManager defaultManager]	createDirectoryAtPath:path withIntermediateDirectories:NO
											   attributes:nil error:&err];
	
	if (err)
		NSLog(@"Unable to create directory at %@, error: %@", path, err);
}

void et::findFiles(const std::string& folder, const std::string& mask, bool /* recursive */, std::vector<std::string>& list)
{
	size_t maskLength = mask.length();
	size_t nameSearchCriteria = mask.find_last_of(".*");
	BOOL searchByName = nameSearchCriteria == maskLength - 1;
	BOOL searchByExt = mask.find_first_of("*.") == 0;
	
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

void et::findSubfolders(const std::string& folder, bool recursive, std::vector<std::string>& list)
{
	NSString* path = [NSString stringWithCString:folder.c_str() encoding:NSASCIIStringEncoding];
	if (![path hasSuffix:@"/"])
		path = [path stringByAppendingString:@"/"];
	
	NSError* err = nil;
	NSArray* files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:&err];
	
	if (err != nil)
	{
		NSLog(@"Unable to findSubfolders: %@", err);
		return;
	}
	
	for (NSString* entry in files)
	{
		BOOL isDir = false;
		NSString* expandedPath = [path stringByAppendingFormat:@"%@/", entry];
 		if ([[NSFileManager defaultManager] fileExistsAtPath:expandedPath isDirectory:&isDir] && isDir)
		{
			std::string entryStr([expandedPath cStringUsingEncoding:NSASCIIStringEncoding]);
			list.push_back(entryStr);
			if (recursive)
				findSubfolders(entryStr, true, list);
		}
	}
}

void et::openUrl(const std::string& url)
{
	if (url.size() == 0) return;
	
	NSURL* aUrl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
	
#if (ET_PLATFORM_IOS)
	[[UIApplication sharedApplication] openURL:aUrl];
#else
	[[NSWorkspace sharedWorkspace] openURL:aUrl];
#endif
}

std::string et::unicodeToUtf8(const std::wstring& w)
{
	NSString* s = [[[NSString alloc] initWithBytes:w.c_str() length:w.length() * sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding] autorelease];
	
	if (s == nil)
	{
		NSLog(@"Unable to convert wstring to NSString.");
		return std::string();
	}
	
	if (![s canBeConvertedToEncoding:NSUTF8StringEncoding])
	{
		NSLog(@"Unable to convert %@ to NSUTF8StringEncoding", s);
		return std::string();
	}
	
	NSUInteger actualLength = 0;
	
	[s getBytes:0 maxLength:0 usedLength:&actualLength
	   encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0, [s length]) remainingRange:0];
	
	BinaryDataStorage result(actualLength + sizeof(char), 0);
	
	[s getBytes:result.data() maxLength:result.dataSize() usedLength:0
	   encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0, [s length]) remainingRange:0];
	
	return std::string(reinterpret_cast<const char*>(result.data()));
}

std::wstring et::utf8ToUnicode(const std::string& mbcs)
{
	NSString* s = [NSString stringWithUTF8String:mbcs.c_str()];
	
	if (s == nil)
	{
		NSLog(@"Unable to convert UTF-8 `%s` to NSString.", mbcs.c_str());
		return std::wstring();
	}
	
	if (![s canBeConvertedToEncoding:NSUTF32LittleEndianStringEncoding])
	{
		NSLog(@"Unable to convert %@ to NSUTF32LittleEndianStringEncoding", s);
		return std::wstring();
	}
	
	NSUInteger actualLength = 0;
    
	[s getBytes:0 maxLength:0 usedLength:&actualLength
	   encoding:NSUTF32LittleEndianStringEncoding options:0 range:NSMakeRange(0, [s length]) remainingRange:0];
	
	BinaryDataStorage result(actualLength + sizeof(wchar_t), 0);
	
	[s getBytes:result.data() maxLength:result.dataSize() usedLength:0
	   encoding:NSUTF32LittleEndianStringEncoding options:0 range:NSMakeRange(0, [s length]) remainingRange:0];
	
	return std::wstring(reinterpret_cast<const wchar_t*>(result.data()));
}

std::string et::applicationIdentifierForCurrentProject()
{
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	return std::string(CFStringGetCStringPtr(CFBundleGetIdentifier(mainBundle), kCFStringEncodingMacRoman));
}
