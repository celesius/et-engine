/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Foundation/NSNotification.h>
#include <et/input/input.h>

using namespace et;

NSString* etKeyboardRequiredNotification = @"etKeyboardRequiredNotification";
NSString* etKeyboardNotRequiredNotification = @"etKeyboardNotRequiredNotification";

PointerInputInfo Input::currentPointer() const
{
	return PointerInputInfo(PointerType_None, vec2(0.0f), vec2(0.0f), vec2(0.0f), 0, queryTime());
}

bool Input::canGetCurrentPointerInfo() const
{
	return false;
}

void Input::activateSoftwareKeyboard()
{
	[[NSNotificationCenter defaultCenter]
		postNotificationName:etKeyboardRequiredNotification object:nil];
}

void Input::deactivateSoftwareKeyboard()
{
	[[NSNotificationCenter defaultCenter]
		postNotificationName:etKeyboardNotRequiredNotification object:nil];
}
