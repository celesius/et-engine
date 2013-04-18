/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <ApplicationServices/ApplicationServices.h>
#include <et/input/input.h>

using namespace et;

bool Input::canGetCurrentPointerInfo() const
{
	return true;
}

PointerInputInfo Input::currentPointer() const
{
	CGEventRef event = CGEventCreate(nil);
	CGPoint loc = CGEventGetLocation(event);
	CFRelease(event);
	
	PointerInputInfo result;
	result.timestamp = queryTime();
	result.pos = vec2(loc.x, loc.y);
	return result;
}

void Input::activateSoftwareKeyboard()
{
}

void Input::deactivateSoftwareKeyboard()
{
}
