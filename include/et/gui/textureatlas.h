/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/apiobjects/texture.h>
#include <et/gui/guibase.h>

namespace et
{
	namespace gui
	{
		class TextureAtlas
		{
		public:
			TextureAtlas();
			TextureAtlas(RenderContext* rc, const std::string& filename, TextureCache& cache);
			
			bool loaded() const
			{ return _loaded; }
			
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
}