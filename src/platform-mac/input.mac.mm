/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <AppKit/NSApplication.h>
#include <AppKit/NSWindow.h>
#include <et/input/input.h>

using namespace et;

bool Input::canGetCurrentPointerInfo()
	{ return true; }

PointerInputInfo Input::currentPointer()
{
	CGEventRef event = CGEventCreate(nil);
	CGPoint loc = CGEventGetLocation(event);
	CFRelease(event);

	NSPoint location = [[[NSApplication sharedApplication] keyWindow]
		convertScreenToBase:NSMakePoint(loc.x, loc.y)];
	
	PointerInputInfo result;
	result.timestamp = queryTime();
	result.pos = vec2(location.x, location.y);
	return result;
}

void Input::activateSoftwareKeyboard()
{
}

void Input::deactivateSoftwareKeyboard()
{
}
