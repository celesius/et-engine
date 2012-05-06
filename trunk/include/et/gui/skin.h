/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

/*
#include <et/gui/font.h>
#include <et/gui/guibase.h>

namespace et
{
	namespace gui
	{
		class Skin
		{
		public:
			Skin() { }
			Skin(const Font& font, const Texture& texture) : _font(font), _texture(texture) { }

			inline Texture& texture()
				{ return _texture; }

			inline const Texture& texture() const 
				{ return _texture; }

			inline void setTexture(const Texture& texture)
				{ _texture = texture; }

			inline const Font& font() const
				{ return _font; }

			inline void setFont(const Font& font)
				{ _font = font; }

			inline const ImageDescriptor& viewImageDescriptor() const
				{ return _viewDescriptor; }

			inline void setViewImageDescriptor(const ImageDescriptor& d)
				{ _viewDescriptor = d; }

			inline const ImageDescriptor& buttonImageDescriptor(ElementState s) const
				{ return _buttonDescriptors[s]; }

			inline void setButtonImageDescriptor(ElementState s, const ImageDescriptor& d)
				{ _buttonDescriptors[s] = d; }

			inline const ImageDescriptor& checkboxImageDescriptor(ElementState s) const
				{ return _checkboxDescriptors[s]; }

			void init();

			void serialize(const std::string& file);
			static Skin deserialize(const std::string& file);

		private:
			Font _font;
			Texture _texture;
			StaticDataStorage<ImageDescriptor, ElementState_max> _buttonDescriptors;
			StaticDataStorage<ImageDescriptor, ElementState_max> _checkboxDescriptors;
			ImageDescriptor _viewDescriptor;
		};
	}
}

*/