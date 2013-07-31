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
	NSWindow* keyWindow = [[NSApplication sharedApplication] keyWindow];
	NSRect frame = [keyWindow contentRectForFrameRect:[keyWindow frame]];
	NSPoint location = [keyWindow convertScreenToBase:[NSEvent mouseLocation]];
	
	PointerInputInfo result;
	result.timestamp = queryTime();
	result.pos = vec2(location.x, frame.size.height - location.y);
	result.normalizedPos = result.pos / vec2(frame.size.width, frame.size.height);
	return result;
}

void Input::activateSoftwareKeyboard()
{
}

void Input::deactivateSoftwareKeyboard()
{
}
