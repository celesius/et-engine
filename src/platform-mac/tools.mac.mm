/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
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