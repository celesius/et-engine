/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <et/sound/sound.h>

using namespace et;
using namespace audio;

extern ALCdevice* getSharedDevice();
extern ALCcontext* getSharedContext();

void Manager::nativeInit()
{
}

void Manager::nativeRelease()
{
}
