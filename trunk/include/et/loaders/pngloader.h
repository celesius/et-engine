#pragma once

#include <string>
#include <et/apiobjects/texturedescription.h>

namespace et
{

	class PNGLoader
	{
	public:
		static void loadInfoFromStream(std::istream& stream, TextureDescription& desc);
		static void loadInfoFromFile(const std::string& path, TextureDescription& desc);

		static void loadFromStream(std::istream& stream, TextureDescription& desc);
		static void loadFromFile(const std::string& path, TextureDescription& desc);
	};

}

