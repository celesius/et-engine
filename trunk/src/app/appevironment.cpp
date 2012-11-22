/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/appevironment.h>
#include <et/core/tools.h>

using namespace et;

AppEnvironment::AppEnvironment() : _appPath(et::applicationPath()), _dataFolder(et::applicationDataFolder()),
	_documentsFolder(et::applicationDocumentsFolder())
{
	addSearchPath(_appPath);
	addSearchPath(_dataFolder);
    
    addSearchPath(_appPath + "../");
	addSearchPath(_dataFolder + "../");

	addSearchPath(_appPath + "data/");
	addSearchPath(_appPath + "../data/");
	addSearchPath(_appPath + "../../data/");

	addSearchPath(_appPath + "textures/" );
	addSearchPath(_appPath + "data/textures/" );
	addSearchPath(_appPath + "../data/textures/" );
	addSearchPath(_appPath + "../../data/textures/" );

	addSearchPath(_appPath + "shaders/" );
	addSearchPath(_appPath + "data/shaders/" );
	addSearchPath(_appPath + "../data/shaders/" );
	addSearchPath(_appPath + "../../data/shaders/" );

	addSearchPath(_appPath + "ui/" );
	addSearchPath(_appPath + "data/ui/" );
	addSearchPath(_appPath + "../data/ui/" );
	addSearchPath(_appPath + "../../data/ui/" );
}

std::string AppEnvironment::findFile(const std::string& name) const
{ 
	if (fileExists(name)) return name;

	for (const std::string& i : _searchPath)
	{
		std::string currentName = normalizeFilePath(i + name);
		if (fileExists(currentName))
			return currentName;
	}

	return name;
}

std::string AppEnvironment::findFolder(const std::string& name) const
{
	if (folderExists(name)) return name;
	
	for (const std::string& i : _searchPath)
	{
		std::string currentName = normalizeFilePath(i + name);
		if (folderExists(currentName))
			return currentName;
	}
	
	return name;	
}

bool AppEnvironment::expandFileName(std::string& name) const
{
	for (const std::string& i : _searchPath)
	{
		const std::string currentName = i + name;
		if (fileExists(currentName))
		{
			name = currentName;
			return true;
		}
	}

	return false;
}

std::string AppEnvironment::resolveScalableFileName(const std::string& name, size_t scale) const
{
	std::string foundFile = findFile(name);
	if ((foundFile.find_last_of("@") != std::string::npos) && fileExists(foundFile))
		return foundFile;

	std::string baseName = removeFileExt(foundFile);
	std::string ext = getFileExt(foundFile);
	while (scale >= 1)
	{
		std::string newFile = baseName + "@" + intToStr(scale) + "x." + ext;
		if (fileExists(newFile))
			return newFile;

		if (scale == 1)
		{
			newFile = baseName + "." + ext;
			if (fileExists(newFile))
				return newFile;
		}

		scale -= 1;
	}

	return foundFile;
}