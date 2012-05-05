//
//  input.mac.mm
//  macosx
//
//  Created by Sergey Reznik on 5/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

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
