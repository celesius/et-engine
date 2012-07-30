/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <sys/time.h>
#include <et/platform/platform.h>
#include <et/core/tools.h>

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
	timeval tv;
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
    while ((
            i < 256) && result[++i]) { };
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
    BOOL exists = false;
    
    NSString* path = [[NSString alloc] initWithCString:name.c_str() encoding:NSASCIIStringEncoding];
    exists = [[NSFileManager defaultManager] fileExistsAtPath:path];
    [path release];
    
	return exists;
}

bool et::folderExists(const std::string& name)
{
	BOOL isDir = false;
	
    NSString* path = [[NSString alloc] initWithCString:name.c_str() encoding:NSASCIIStringEncoding];
	[[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    [path release];
	
	return isDir;
}

std::string et::applicationDocumentsFolder()
{
    @autoreleasepool 
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* folder = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
        return std::string([folder cStringUsingEncoding:NSASCIIStringEncoding]);
    }
}

void et::createDirectory(const std::string& name)
{
	NSError* err = nil;
	NSString* path = [NSString stringWithCString:name.c_str() encoding:NSASCIIStringEncoding];
	[[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:NO attributes:nil error:&err];
	
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
	NSURL* aUrl = [NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSASCIIStringEncoding]];
	
#if (ET_PLATFORM_IOS)
	[[UIApplication sharedApplication] openURL:aUrl];
#else
	[[NSWorkspace sharedWorkspace] openURL:aUrl];
#endif
}

std::string et::unicodeToUtf8(const std::wstring& w)
{
	NSString* s = [[[NSString alloc] initWithBytes:w.c_str() length:w.length() * sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding] autorelease];
	return std::string([s cStringUsingEncoding:NSUTF8StringEncoding]);
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
	
	return std::wstring(reinterpret_cast<const wchar_t*>([s cStringUsingEncoding:NSUTF32LittleEndianStringEncoding]));
}