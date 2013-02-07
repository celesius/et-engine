#include <CoreText/CoreText.h>

#include <et/rendering/rendercontext.h>
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

		void updateTexture(RenderContext* rc, const vec2i& position, const vec2i& size,
			Texture texture, BinaryDataStorage& data);
	
		void renderCharacter(NSString* value, const vec2i& position, const vec2i& size,
			bool bold, BinaryDataStorage& data);

	public:
		std::string _fontFace;
		size_t _fontSize;
	
		RectPlacer _placer;
   
        UIFont* _font;
        UIFont* _boldFont;
        CGContextRef _context;
		CGColorSpaceRef _colorSpace;
};

CharacterGenerator::CharacterGenerator(RenderContext* rc, const std::string& face,
	const std::string& boldFace, size_t size) : _rc(rc),
	_private(new CharacterGeneratorPrivate(face, boldFace, size)), _face(face), _size(size)
{
	_texture = _rc->textureFactory().genTexture(GL_TEXTURE_2D, GL_RGBA, vec2i(defaultTextureSize),
		GL_RGBA, GL_UNSIGNED_BYTE, BinaryDataStorage(), face + "font");
}

CharacterGenerator::~CharacterGenerator()
{
	delete _private;
}

CharDescriptor CharacterGenerator::generateCharacter(int value, bool updateTexture)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytes:string length:sizeof(string)
											   encoding:NSUTF32LittleEndianStringEncoding];
    CGSize characterSize = [wString sizeWithFont:_private->_font];
	vec2i charSize = vec2i(characterSize.width, characterSize.height);
	
	BinaryDataStorage data(charSize.square() * 4, 0);
	
	rect textureRect;
	_private->_placer.place(charSize + vec2i(2), textureRect);
	vec2i position = vec2i(static_cast<int>(textureRect.left + 1.0f), static_cast<int>(textureRect.top + 1.0f));
	_private->renderCharacter(wString, position, charSize, false, data);
	_private->updateTexture(_rc, position, charSize, _texture, data);
	
	[wString release];
	
	CharDescriptor desc(value);
	
	desc.origin = textureRect.origin() + vec2(1.0f);
	desc.size = textureRect.size() - vec2(2.0f);
	desc.uvOrigin = _texture->getTexCoord(desc.origin);
	desc.uvSize = desc.size / _texture->sizeFloat();
	
	_chars[value] = desc;
	return desc;
}

CharDescriptor CharacterGenerator::generateBoldCharacter(int value, bool updateTexture)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytes:string length:sizeof(string)
											   encoding:NSUTF32LittleEndianStringEncoding];
	
    CGSize characterSize = [wString sizeWithFont:_private->_boldFont];
	vec2i charSize = vec2i(characterSize.width, characterSize.height);
	
	BinaryDataStorage data(charSize.square() * 4, 0);

	rect textureRect;
	_private->_placer.place(charSize + vec2i(2), textureRect);
	vec2i position = vec2i(static_cast<int>(textureRect.left + 1.0f), static_cast<int>(textureRect.top + 1.0f));
	_private->renderCharacter(wString, position, charSize, true, data);
	_private->updateTexture(_rc, position, charSize, _texture, data);
	
	[wString release];
	
	CharDescriptor desc(value, CharParameter_Bold);
	
	desc.origin = textureRect.origin() + vec2(1.0f);
	desc.size = textureRect.size() - vec2(2.0f);
	desc.uvOrigin = _texture->getTexCoord(desc.origin);
	desc.uvSize = desc.size / _texture->sizeFloat();
	
	_boldChars[value] = desc;
	return desc;
}

/*
 * Private
 */

CharacterGeneratorPrivate::CharacterGeneratorPrivate(const std::string& face,
	const std::string& boldFace, size_t size) : _fontFace(face), _fontSize(size),
	_placer(vec2i(defaultTextureSize), true)
{
    NSString* cFace = [NSString stringWithCString:face.c_str() encoding:NSASCIIStringEncoding];
    NSString* cBoldFace = [NSString stringWithCString:boldFace.c_str() encoding:NSASCIIStringEncoding];
	
    _font = [[UIFont fontWithName:cFace size:static_cast<float>(size)] retain];
	assert(_font);
	
    _boldFont = [[UIFont fontWithName:cBoldFace size:static_cast<float>(size)] retain];
	assert(_boldFont);
}

CharacterGeneratorPrivate::~CharacterGeneratorPrivate()
{
    [_font release];
	[_boldFont release];
}

void CharacterGeneratorPrivate::updateTexture(RenderContext* rc, const vec2i& position,
	const vec2i& size, Texture texture, BinaryDataStorage& data)
{
	vec2i dest(position.x, defaultTextureSize - position.y - size.y);
	texture->updatePartialDataDirectly(rc, dest, size, data.binary(), data.dataSize());
}

void CharacterGeneratorPrivate::renderCharacter(NSString* value, const vec2i& position,
	const vec2i& size, bool bold, BinaryDataStorage& data)
{
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	assert(colorSpace);
	
	CGContextRef context = CGBitmapContextCreateWithData(data.data(), size.x, size.y, 8,
		4 * size.x, colorSpace, kCGImageAlphaPremultipliedLast, 0, 0);
	assert(context);
	
	CGContextSetTextDrawingMode(context, kCGTextFill);
	CGContextSetFillColorWithColor(context, [[UIColor whiteColor] CGColor]);
	CGContextSetShouldAntialias(context, YES);
	CGContextSetShouldSmoothFonts(context, YES);
	CGContextSetShouldSubpixelPositionFonts(context, YES);
	CGContextSetShouldSubpixelQuantizeFonts(context, NO);
	UIGraphicsPushContext(context);
	
	[value drawAtPoint:CGPointZero withFont:(bold ? _boldFont : _font)];
	
	UIGraphicsPopContext();
	CGContextRelease(context);
	CGColorSpaceRelease(colorSpace);

	unsigned int* ptr = reinterpret_cast<unsigned int*>(data.data());
	unsigned int* endPtr = ptr + data.dataSize() / 4;
	while (ptr != endPtr)
	{
		*ptr++ |= 0x00FFFFFF;
	}
}
