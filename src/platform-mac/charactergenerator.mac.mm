#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>

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
		NSFont* font, BinaryDataStorage& data);
	
public:
	std::string fontFace;
	size_t fontSize;
	
	RectPlacer _placer;
	
	NSFont* font;
	NSFont* boldFont;
		
	CGColorRef whiteColor;
	CGColorSpaceRef colorSpace;
};

CharacterGenerator::CharacterGenerator(RenderContext* rc, const std::string& face,
	const std::string& boldFace, size_t size) : _rc(rc),
	_private(new CharacterGeneratorPrivate(face, boldFace, size)), _face(face), _size(size)
{
	BinaryDataStorage emptyData(defaultTextureSize * defaultTextureSize * 4, 0);
	
	_texture = _rc->textureFactory().genTexture(GL_TEXTURE_2D, GL_RGBA, vec2i(defaultTextureSize),
												GL_RGBA, GL_UNSIGNED_BYTE, emptyData, face + "font");
}

CharacterGenerator::~CharacterGenerator()
{
	delete _private;
}

CharDescriptor CharacterGenerator::generateCharacter(int value, bool updateTexture)
{
	wchar_t string[2] = { value, 0 };
    NSString* wString = [[NSString alloc] initWithBytesNoCopy:string length:sizeof(string)
		encoding:NSUTF32LittleEndianStringEncoding freeWhenDone:NO];
	
	NSDictionary* attrib = [[_private->font fontDescriptor] fontAttributes];
	NSSize characterSize = [wString sizeWithAttributes:attrib];
	vec2i charSize = vec2i(characterSize.width, characterSize.height);
		
	BinaryDataStorage data(charSize.square() * 4, 0);
	
	rect textureRect;
	_private->_placer.place(charSize + vec2i(2), textureRect);
	vec2i position = vec2i(static_cast<int>(textureRect.left + 1.0f), static_cast<int>(textureRect.top + 1.0f));
	_private->renderCharacter(wString, position, charSize, _private->font, data);
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
    NSString* wString = [[NSString alloc] initWithBytesNoCopy:string length:sizeof(string)
		encoding:NSUTF32LittleEndianStringEncoding freeWhenDone:NO];
	
	NSDictionary* attrib = [[_private->boldFont fontDescriptor] fontAttributes];
	NSSize characterSize = [wString sizeWithAttributes:attrib];
	vec2i charSize = vec2i(characterSize.width, characterSize.height);
	
	BinaryDataStorage data(charSize.square() * 4, 0);
	
	rect textureRect;
	_private->_placer.place(charSize + vec2i(2), textureRect);
	vec2i position = vec2i(static_cast<int>(textureRect.left + 1.0f), static_cast<int>(textureRect.top + 1.0f));
	_private->renderCharacter(wString, position, charSize, _private->boldFont, data);
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
	const std::string& boldFace, size_t size) : fontFace(face), fontSize(size),
	_placer(vec2i(defaultTextureSize), true)
{
    NSString* cFace = [NSString stringWithCString:face.c_str() encoding:NSUTF8StringEncoding];
	
	font = [[[NSFontManager sharedFontManager] fontWithFamily:cFace
		traits:0 weight:2 size:size] retain];
	assert(font);
	
	boldFont = [[[NSFontManager sharedFontManager] fontWithFamily:cFace
		traits:NSBoldFontMask weight:2 size:size] retain];
	assert(boldFont);
	
	whiteColor = CGColorCreateGenericRGB(1.0f, 1.0f, 1.0f, 1.0f);
	assert(whiteColor);
	
	colorSpace = CGColorSpaceCreateDeviceRGB();
	assert(colorSpace);
}

CharacterGeneratorPrivate::~CharacterGeneratorPrivate()
{
	CGColorRelease(whiteColor);
	CGColorSpaceRelease(colorSpace);
	
    [font release];
	[boldFont release];
}

void CharacterGeneratorPrivate::updateTexture(RenderContext* rc, const vec2i& position,
	const vec2i& size, Texture texture, BinaryDataStorage& data)
{
	vec2i dest(position.x, defaultTextureSize - position.y - size.y);
	texture->updatePartialDataDirectly(rc, dest, size, data.binary(), data.dataSize());
}

void CharacterGeneratorPrivate::renderCharacter(NSString* value, const vec2i& position,
	const vec2i& size, NSFont* font, BinaryDataStorage& data)
{
	CGContextRef context = CGBitmapContextCreateWithData(data.data(), size.x, size.y, 8, 4 * size.x,
		colorSpace, kCGImageAlphaPremultipliedLast, 0, 0);
	assert(context);
	
	CGContextSetTextDrawingMode(context, kCGTextFill);
	CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 1.0f);
	
	NSDictionary* attrib = [[font fontDescriptor] fontAttributes];
	NSGraphicsContext* aContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext setCurrentContext:aContext];
	[font setInContext:aContext];
	
	NSRect r = NSMakeRect(0.0 * position.x, 0.0 * position.y, size.x, size.y);
	[value drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin attributes:attrib];
	[value drawAtPoint:NSMakePoint(0 * position.x, 0 * position.y) withAttributes:attrib];
	[aContext flushGraphics];
	
	CGContextRelease(context);
	
	unsigned int* ptr = reinterpret_cast<unsigned int*>(data.data());
	unsigned int* endPtr = ptr + data.dataSize() / 4;
	while (ptr != endPtr)
		*ptr++ |= 0x00FFFFFF;
/*
	std::string fn = applicationDocumentsBaseFolder() + "'" + [value cStringUsingEncoding:NSUTF8StringEncoding] + "'.png";
	ImageWriter::writeImageToFile(fn, data, size, 4, 8, ImageFormat_PNG);
*/ 
}
