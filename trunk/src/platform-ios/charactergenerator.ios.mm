#include <et/geometry/rectplacer.h>
#include <et/gui/charactergenerator.h>
#include <et/imaging/imagewriter.h>

using namespace et;
using namespace et::gui;

const int defaultTextureBindUnit = 7;
const int defaultTextureSize = 1024;

class et::gui::CharacterGeneratorPrivate
{
	public:
		CharacterGeneratorPrivate(const std::string& face, size_t size);
		~CharacterGeneratorPrivate();

		void updateTexture(RenderContext* rc, Texture texture);
		void renderCharacter(int value, const vec2i& position);

	public:
		RectPlacer _placer;
        BinaryDataStorage _data;
    
        UIFont* _font;
        UIFont* _boldFont;
        CGContextRef _context;
};

CharacterGenerator::CharacterGenerator(RenderContext* rc, const std::string& face, size_t size) : _rc(rc),
	_private(new CharacterGeneratorPrivate(face, size)), _face(face), _size(size)
{
	_texture = _rc->textureFactory().genTexture(GL_TEXTURE_2D, GL_RGBA, vec2i(defaultTextureSize), GL_RGBA, GL_UNSIGNED_BYTE, 
		BinaryDataStorage(), face + "font");
}

CharacterGenerator::~CharacterGenerator()
{
	delete _private;
}

CharDescriptor CharacterGenerator::generateCharacter(int value)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytes:string length:sizeof(string) encoding:NSUTF32LittleEndianStringEncoding];

    CGSize characterSize = [wString sizeWithFont:[UIFont systemFontOfSize:17]];
     
	rect textureRect;
	_private->_placer.place(vec2i(characterSize.width + 2, characterSize.height + 2), textureRect);
	_private->renderCharacter(value, vec2i(static_cast<int>(textureRect.left + 1), static_cast<int>(textureRect.top + 1)));
	_private->updateTexture(_rc, _texture);

	std::cout << "Value for `" << value << "` placed to " << textureRect.origin() << std::endl;

	CharDescriptor desc(value);
	desc.origin = textureRect.origin() + vec2(1.0f);
	desc.size = textureRect.size() - vec2(2.0f);
	desc.uvOrigin = _texture->getTexCoord(textureRect.origin());
	desc.uvSize = textureRect.size() / _texture->sizeFloat();
	_chars[value] = desc;
	return desc;
}

CharDescriptor CharacterGenerator::generateBoldCharacter(int value)
{
	CharDescriptor desc(value, CharParameter_Bold);

	rect t;
	_private->_placer.place(vec2i(10, 10), t);

	_boldChars[value] = desc;
	return desc;
}


/*
 * Private
 */

CharacterGeneratorPrivate::CharacterGeneratorPrivate(const std::string& face, size_t size) : 
	_placer(vec2i(defaultTextureSize), true)
{
    NSString* cFace = [NSString stringWithCString:face.c_str() encoding:NSASCIIStringEncoding];
    _font = [[UIFont fontWithName:cFace size:static_cast<float>(size)] retain];
    
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    _context = CGBitmapContextCreate(_data.data(), defaultTextureSize, defaultTextureSize, 
                                                 8, defaultTextureSize * 4, cs, kCGImageAlphaLast);
}

CharacterGeneratorPrivate::~CharacterGeneratorPrivate()
{
    [_font release];
}

void CharacterGeneratorPrivate::updateTexture(RenderContext* rc, Texture texture)
{
	texture->updateDataDirectly(rc, vec2i(defaultTextureSize), static_cast<char*>(_data.raw()), _data.size());
}

void CharacterGeneratorPrivate::renderCharacter(int value, const vec2i& position)
{
}
