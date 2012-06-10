/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#pragma once

#include <string>
#include <et/core/intrusiveptr.h>
#include <et/gui/fontbase.h>
#include <et/rendering/rendercontext.h>
#include <et/apiobjects/texture.h>

namespace et
{
	namespace gui
	{
		class CharacterGeneratorPrivate;
		class CharacterGenerator : public Shared
		{
		public:
			typedef et::IntrusivePtr<CharacterGenerator> Pointer;

		public:
			CharacterGenerator(RenderContext* _rc, const std::string& face, size_t size);
			~CharacterGenerator();

			const Texture& texture() const 
				{ return _texture; }

			size_t size() const
				{ return _size; }

			const std::string& face() const
				{ return _face; }

			CharDescriptor charDescription(int c)
			{
				CharDescriptorMap::const_iterator i = _chars.find(c);
				return (i != _chars.end()) ? i->second : generateCharacter(c);
			}

			CharDescriptor boldCharDescription(int c)
			{
				CharDescriptorMap::const_iterator i = _boldChars.find(c);
				return (i != _boldChars.end()) ? i->second : generateBoldCharacter(c);
			}

			float lineHeight() const
				{ return _chars.size() ? _chars.begin()->second.size.y : static_cast<float>(_size); }

		private:
			CharDescriptor generateCharacter(int value);
			CharDescriptor generateBoldCharacter(int value);

		private:
			RenderContext* _rc;
			CharacterGeneratorPrivate* _private;

			Texture _texture;
			CharDescriptorMap _chars;
			CharDescriptorMap _boldChars;

			std::string _face;
			size_t _size;
		};

	}

}