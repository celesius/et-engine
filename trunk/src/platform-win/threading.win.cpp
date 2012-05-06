/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <Pdh.h>
#include <et/threading/threading.h>

using namespace et;

ThreadId Threading::_mainThread;

Threading::Threading()
{
	_mainThread = GetCurrentThreadId();
}

ThreadId Threading::currentThread()
{
	return GetCurrentThreadId();
}

size_t Threading::coresCount()
{
	SYSTEM_INFO info = { };
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

double Threading::cpuUsage()
{
	static ULONGLONG idle = 0;
	static ULONGLONG kern = 0;
	static ULONGLONG user = 0;

	ULONGLONG idle_c = 0;
	ULONGLONG kern_c = 0;
	ULONGLONG user_c = 0;
	
	GetSystemTimes(reinterpret_cast<LPFILETIME>(&idle_c), reinterpret_cast<LPFILETIME>(&kern_c), reinterpret_cast<LPFILETIME>(&user_c));

	ULONGLONG dIdle = idle_c - idle;
	ULONGLONG dSys = (user_c - user) + (kern_c - kern);

	idle = idle_c;
	kern = kern_c;
	user = user_c;

	return static_cast<double>(dSys - dIdle) / static_cast<double>(dSys) * 100.0;
}