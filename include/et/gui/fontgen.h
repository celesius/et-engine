#pragma once

#include <et/gui/fontbase.h>
#include <et/utils/imagewriter.h>

namespace et
{
	namespace gui
	{
		enum FontGeneratorResult
		{
			FontGeneratorResult_Success,
			FontGeneratorResult_OutputFileNotDefined,
			FontGeneratorResult_OutputFileFailed,
		};

		class FontGenerator
		{
		public:
			FontGenerator();

			inline void setFontFace(const std::string& face)
				{ _face = face; }

			inline void setSize(int size)
				{ _size = size; }

			inline void setCharacterRange(const CharacterRange& range)
				{ _characterRange = range; }

			inline void setOffset(float offset)
				{ _offset = offset; }

			void setOutputFile(const std::string& fileName);

			FontGeneratorResult generate(ImageFormat fmt = ImageFormat_PNG);

		private:
			void fillCharacterDescriptors(const std::string& face, int size, bool bold, CharDescriptorList& chars, const CharacterRange& range, 
				const vec2& extraOffsets = vec2(0.0f));

		private:
			std::string _face;
			std::string _outFile;
			CharacterRange _characterRange;
			int _size;
			float _offset;
			bool _outFileSet;
		};
	}
}
