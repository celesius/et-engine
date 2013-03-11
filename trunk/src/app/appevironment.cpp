/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/tools.h>
#include <et/app/application.h>
#include <et/app/appevironment.h>

using namespace et;

AppEnvironment::AppEnvironment() :
	_appPath(et::applicationPath()),
	_appPackagePath(et::applicationPackagePath()),
	_dataFolder(et::applicationDataFolder())
{
	addSearchPath(_appPath);

	addSearchPath(_dataFolder);
    addSearchPath(_dataFolder + "../");
	addSearchPath(_dataFolder + "../../");

	addSearchPath(_dataFolder + "data/");
	addSearchPath(_dataFolder + "../data/");
	addSearchPath(_dataFolder + "../../data/");

	addSearchPath(_dataFolder + "textures/" );
	addSearchPath(_dataFolder + "data/textures/" );
	addSearchPath(_dataFolder + "../data/textures/" );
	addSearchPath(_dataFolder + "../../data/textures/" );

	addSearchPath(_dataFolder + "shaders/" );
	addSearchPath(_dataFolder + "data/shaders/" );
	addSearchPath(_dataFolder + "../data/shaders/" );
	addSearchPath(_dataFolder + "../../data/shaders/" );
}

const std::string& AppEnvironment::applicationDocumentsFolder() const
{
	assert(!_documentsFolder.empty());
	return _documentsFolder;
}

void AppEnvironment::updateDocumentsFolder(const ApplicationIdentifier& i)
{
	_documentsFolder = applicationLibraryBaseFolder() + i.companyName;
	if (!folderExists(_documentsFolder))
		createDirectory(_documentsFolder);

	assert(folderExists(_documentsFolder));

	_documentsFolder += pathDelimiter + i.applicationName;
	if (!folderExists(_documentsFolder))
		createDirectory(_documentsFolder);

	assert(folderExists(_documentsFolder));
}

void AppEnvironment::addSearchPath(const std::string& path)
{
	_searchPath.push_back(addTrailingSlash(path));
}

void AppEnvironment::addRelativeSearchPath(const std::string& path)
{
	_searchPath.push_back(addTrailingSlash(_appPath + path));
}

std::string AppEnvironment::findFile(const std::string& name) const
{
	if (fileExists(name)) return name;

	ET_ITERATE(_searchPath, const std::string&, i,
	{
		std::string currentName = normalizeFilePath(i + name);
		if (fileExists(currentName))
			return currentName;
	})

	return name;
}

std::string AppEnvironment::findFolder(const std::string& name) const
{
	if (folderExists(name)) return name;
	
	ET_ITERATE(_searchPath, const std::string&, i,
	{
		std::string currentName = normalizeFilePath(i + name);
		if (folderExists(currentName))
			return currentName;
	})
	
	return name;	
}

bool AppEnvironment::expandFileName(std::string& name) const
{
	ET_ITERATE(_searchPath, const std::string&, i,
	{
		const std::string currentName = i + name;
		if (fileExists(currentName))
		{
			name = currentName;
			return true;
		}
	})

	return false;
}

std::string AppEnvironment::resolveScalableFileName(const std::string& name, size_t scale) const
{
	assert(scale > 0);
	std::string foundFile = findFile(name);

	if ((foundFile.find_last_of("@") != std::string::npos) && fileExists(foundFile))
		return foundFile;

	std::string baseName = removeFileExt(foundFile);
	std::string ext = getFileExt(foundFile);

	while (scale >= 1)
	{
		std::string newFile = findFile(baseName + "@" + intToStr(scale) + "x." + ext);
		if (fileExists(newFile))
			return newFile;

		if (scale == 1)
		{
			newFile = findFile(baseName + "." + ext);
			if (fileExists(newFile))
				return newFile;
		}

		scale -= 1;
	}

	return foundFile;
}
