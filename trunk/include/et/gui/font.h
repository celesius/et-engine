/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <string>
#include <et/core/intrusiveptr.h>
#include <et/rendering/rendercontext.h>
#include <et/apiobjects/texture.h>
#include <et/gui/fontbase.h>

namespace et
{
	namespace gui
	{
		class FontData : public Shared
		{
		public:
			FontData();
			FontData(RenderContext* rc, const std::string& fileName, TextureCache& cache);
			~FontData();

			void loadFromFile(RenderContext* rc, const std::string& fileName, TextureCache& cache);

			inline const Texture& texture() const 
				{ return _texture; }

			const CharDescriptor& charDescription(short c) const;
			const CharDescriptor& boldCharDescription(short c) const;

			float lineHeight() const;

			int size() const
				{ return _size; }

			const std::string& face() const
				{ return _face; }

			CharDescriptorList buildString(const std::string& s, bool formatted = false) const;

			vec2 measureStringSize(const std::string& s, bool formatted) const;
			vec2 measureStringSize(const CharDescriptorList& s) const;

		private:
			CharDescriptorList parseString(const std::string& s) const;

		private:
			Texture _texture;
			CharDescriptorMap _chars;
			CharDescriptorMap _boldChars;
			std::string _face;
			int _size;
		};

		typedef IntrusivePtr<FontData> SharedFontPtr;

		class Font : public SharedFontPtr
		{
		public:
			Font() : SharedFontPtr(0) 
				{ }

			Font(RenderContext* rc, const std::string& fileName, TextureCache& cache) : 
				SharedFontPtr(new FontData(rc, fileName, cache)) { }
		};

	}
}