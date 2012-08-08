#include <CoreText/CoreText.h>

#include <et/geometry/rectplacer.h>
#include <et/gui/charactergenerator.h>
#include <et/imaging/imagewriter.h>

using namespace et;
using namespace et::gui;

const size_t defaultTextureSize = 1024;

class et::gui::CharacterGeneratorPrivate
{
	public:
		CharacterGeneratorPrivate(const std::string& face, const std::string& boldFace, size_t size);
		~CharacterGeneratorPrivate();

		void updateTexture(RenderContext* rc, Texture texture);
		void renderCharacter(NSString* value, const vec2i& position, bool bold);

	public:
		std::string _fontFace;
		size_t _fontSize;
	
		RectPlacer _placer;
        BinaryDataStorage _data;
    
        UIFont* _font;
        UIFont* _boldFont;
        CGContextRef _context;
		CGColorSpaceRef _colorSpace;
};

CharacterGenerator::CharacterGenerator(RenderContext* rc, const std::string& face, const std::string& boldFace, size_t size) : _rc(rc),
	_private(new CharacterGeneratorPrivate(face, boldFace, size)), _face(face), _size(size)
{
	_texture = _rc->textureFactory().genTexture(GL_TEXTURE_2D, GL_RGBA, vec2i(defaultTextureSize), GL_RGBA, GL_UNSIGNED_BYTE, 
		BinaryDataStorage(), face + "font");
	
	for (size_t i = 32; i < 128; ++i)
		generateCharacter(i, false);

	for (size_t i = 32; i < 128; ++i)
		generateBoldCharacter(i, false);
	
	_private->updateTexture(rc, _texture);
}

CharacterGenerator::~CharacterGenerator()
{
	delete _private;
}

CharDescriptor CharacterGenerator::generateCharacter(int value, bool updateTexture)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytes:string length:sizeof(string) encoding:NSUTF32LittleEndianStringEncoding];
    CGSize characterSize = [wString sizeWithFont:_private->_font];

	rect textureRect;
	_private->_placer.place(vec2i(characterSize.width + 2, characterSize.height + 2), textureRect);
	_private->renderCharacter(wString, vec2i(static_cast<int>(textureRect.left + 1), static_cast<int>(textureRect.top + 1)), false);
	
	if (updateTexture)
		_private->updateTexture(_rc, _texture);

	[wString release];
	
	CharDescriptor desc(value);
	
	desc.origin = textureRect.origin() + vec2(1.0f);
	desc.size = textureRect.size() - vec2(2.0f);
	desc.uvOrigin = _texture->getTexCoord(textureRect.origin());
	desc.uvSize = textureRect.size() / _texture->sizeFloat();
	
	_chars[value] = desc;
	return desc;
}

CharDescriptor CharacterGenerator::generateBoldCharacter(int value, bool updateTexture)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytes:string length:sizeof(string) encoding:NSUTF32LittleEndianStringEncoding];
    CGSize characterSize = [wString sizeWithFont:_private->_boldFont];
	
	rect textureRect;
	_private->_placer.place(vec2i(characterSize.width + 2, characterSize.height + 2), textureRect);
	_private->renderCharacter(wString, vec2i(static_cast<int>(textureRect.left + 1), static_cast<int>(textureRect.top + 1)), true);
	
	if (updateTexture)
		_private->updateTexture(_rc, _texture);
	
	[wString release];
	
	CharDescriptor desc(value, CharParameter_Bold);
	
	desc.origin = textureRect.origin() + vec2(1.0f);
	desc.size = textureRect.size() - vec2(2.0f);
	desc.uvOrigin = _texture->getTexCoord(textureRect.origin());
	desc.uvSize = textureRect.size() / _texture->sizeFloat();
	
	_boldChars[value] = desc;
	return desc;
}

/*
 * Private
 */

CharacterGeneratorPrivate::CharacterGeneratorPrivate(const std::string& face, const std::string& boldFace, size_t size) :
	_fontFace(face), _fontSize(size),
	_placer(vec2i(defaultTextureSize), true), _data(defaultTextureSize * defaultTextureSize * 4, 0)
{
    NSString* cFace = [NSString stringWithCString:face.c_str() encoding:NSASCIIStringEncoding];
    NSString* cBoldFace = [NSString stringWithCString:boldFace.c_str() encoding:NSASCIIStringEncoding];
	
    _font = [[UIFont fontWithName:cFace size:static_cast<float>(size)] retain];
    _boldFont = [[UIFont fontWithName:cBoldFace size:static_cast<float>(size)] retain];
	
	_colorSpace = CGColorSpaceCreateDeviceRGB();
	
    _context = CGBitmapContextCreate(_data.data(), defaultTextureSize, defaultTextureSize,
									 8, defaultTextureSize * 4, _colorSpace, kCGImageAlphaPremultipliedLast);

	CGContextSetTextDrawingMode(_context, kCGTextFill);
	CGContextSetFillColorWithColor(_context, [[UIColor whiteColor] CGColor]);
	CGContextSetShouldAntialias(_context, YES);
	CGContextSetShouldSmoothFonts(_context, YES);
}

CharacterGeneratorPrivate::~CharacterGeneratorPrivate()
{
	CGContextRelease(_context);
	CGColorSpaceRelease(_colorSpace);
    [_font release];
}

void CharacterGeneratorPrivate::updateTexture(RenderContext* rc, Texture texture)
{
	for (size_t i = 0; i < _data.size() / 4; ++i)
	{
		_data[4*i+0] = 255;
		_data[4*i+1] = 255;
		_data[4*i+2] = 255;
	}
	texture->updateDataDirectly(rc, vec2i(defaultTextureSize), _data.binary(), _data.size());
}

void CharacterGeneratorPrivate::renderCharacter(NSString* value, const vec2i& position, bool bold)
{
	UIGraphicsPushContext(_context);
	[value drawAtPoint:CGPointMake(position.x, position.y) withFont:(bold ? _boldFont : _font)];
	UIGraphicsPopContext();
}
