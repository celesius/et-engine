#pragma once

#include <et/apiobjects/texture.h>
#include <et/gui/guibase.h>

namespace et
{
	class TextureAtlas
	{
	public:
		TextureAtlas();
		TextureAtlas(RenderContext* rc, const std::string& filename, TextureCache& cache);

		void loadFromFile(RenderContext* rc, const std::string& filename, TextureCache& cache);
		void unload();

		const gui::Image& image(const std::string& key) const;
		gui::ImageList imagesForTexture(Texture t) const;

	private:
		static gui::Image _emptyImage;

		TextureMap _textures;
		gui::ImageMap _images;
		bool _loaded;
	};
}