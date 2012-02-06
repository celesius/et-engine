#pragma once

#include <string>
#include <et/scene3d/scene3d.h>

namespace et
{
	class RenderContext;
	class FBXLoader
	{
	public:
		FBXLoader(const std::string& filename);
		~FBXLoader();

		s3d::ElementContainer::Pointer load(RenderContext* rc, TextureCache& textureCache);

	private:
		std::string _filename;
	};
}