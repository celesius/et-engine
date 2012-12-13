/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <Windows.h>
#include <et/platform/platformtools.h>

using namespace et;

std::string et::selectFile(const StringList&, SelectFileMode mode)
{
	char filename[1024] = { };

	OPENFILENAME of = { };
	of.lStructSize = sizeof(of);
	of.hwndOwner = 0;
	of.hInstance = GetModuleHandle(0);
	of.lpstrFilter = "All supported files\0*.fbx;*.etm\0FBX files\0*.fbx\0ET models\0*.etm\0\0";
	of.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	of.lpstrFile = filename;
	of.nMaxFile = MAX_PATH;

	if (mode == SelectFileMode_Open)
		return GetOpenFileName(&of) ? std::string(filename) : std::string();
	else 
		return GetSaveFileName(&of) ? std::string(filename) : std::string();
}
