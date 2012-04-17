#pragma once

#include <string>
#include <et/apiobjects/texturedescription.h>

namespace et
{
    struct PVRHeader2;
    struct PVRHeader3;
    
	class PVRLoader
	{
	public:
		static void loadInfoFromStream(std::istream& stream, TextureDescription& desc);
		static void loadInfoFromFile(const std::string& path, TextureDescription& desc);

		static void loadFromStream(std::istream& stream, TextureDescription& desc);
		static void loadFromFile(const std::string& path, TextureDescription& desc);
        
    private:
        static void loadInfoFromV2Header(const PVRHeader2& header, TextureDescription& desc);
        static void loadInfoFromV3Header(const PVRHeader3& header, TextureDescription& desc);
	};

}

