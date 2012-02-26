#include <string>
#include <et/core/tools.h>

#pragma once

namespace et
{
	class AppEnvironment
	{
	public: 
		AppEnvironment();

		inline const std::string& applicationPath() const
			{ return _appPath; }

		inline const std::string& applicationInputDataFolder() const
			{ return _dataFolder; }
		
		inline const std::string& applicationDocumentsFolder() const
			{ return _documentsFolder; }

		bool expandFileName(std::string& name) const;

		std::string findFile(const std::string& name) const;
		std::string findFolder(const std::string& name) const;
		std::string resolveScalableFileName(const std::string& name, size_t scale) const;

		void addSearchPath(const std::string& path)
			{ _searchPath.push_back(addTrailingSlash(path)); }

		void addRelativeSearchPath(const std::string& path)
			{ _searchPath.push_back(addTrailingSlash(_appPath + path)); }

	private:
		std::string _appPath;
		std::string _dataFolder;
		std::string _documentsFolder;
		StringList _searchPath;
	};
}