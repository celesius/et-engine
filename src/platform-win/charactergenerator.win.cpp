#include <Windows.h>
#include <et/geometry/rectplacer.h>
#include <et/gui/charactergenerator.h>
#include <et/imaging/imagewriter.h>
#include <et/timers/intervaltimer.h>

using namespace et;
using namespace et::gui;

const int defaultTextureBindUnit = 7;
const int defaultTextureSize = 1024;

class et::gui::CharacterGeneratorPrivate
{
	public:
		CharacterGeneratorPrivate(const std::string& face, const std::string& boldFace, size_t size);
		~CharacterGeneratorPrivate();

		void updateTexture(RenderContext* rc, Texture texture);
		void renderCharacter(int value, const vec2i& position);
		void renderBoldCharacter(int value, const vec2i& position);

	public:
		size_t _size;		
		std::string _face;
		std::string _boldFace;

		RectPlacer _placer;

		HDC _dc;
		HFONT _font;
		HFONT _boldFont;
		HBITMAP _bitmap;
};

CharacterGenerator::CharacterGenerator(RenderContext* rc, const std::string& face, const std::string& boldFace, size_t size) : _rc(rc),
	_private(new CharacterGeneratorPrivate(face, boldFace, size)), _face(face), _boldFace(boldFace), _size(size)
{
	_texture = _rc->textureFactory().genTexture(GL_TEXTURE_2D, GL_RGBA, vec2i(defaultTextureSize), GL_RGBA, 
		GL_UNSIGNED_BYTE, BinaryDataStorage(), face + "font");

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
	SIZE characterSize = { };
	wchar_t string[2] = { value, 0 };

	SelectObject(_private->_dc, _private->_font);
	GetTextExtentPointW(_private->_dc, string, 1, &characterSize);

	rect textureRect;
	_private->_placer.place(vec2i(characterSize.cx + 2, characterSize.cy + 2), textureRect);
	_private->renderCharacter(value, vec2i(static_cast<int>(textureRect.left + 1), static_cast<int>(textureRect.top + 1)));

	if (updateTexture)
		_private->updateTexture(_rc, _texture);

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
	SIZE characterSize = { };
	wchar_t string[2] = { value, 0 };

	SelectObject(_private->_dc, _private->_boldFont);
	GetTextExtentPointW(_private->_dc, string, 1, &characterSize);

	rect textureRect;
	_private->_placer.place(vec2i(characterSize.cx + 2, characterSize.cy + 2), textureRect);
	_private->renderBoldCharacter(value, vec2i(static_cast<int>(textureRect.left + 1), static_cast<int>(textureRect.top + 1)));

	if (updateTexture)
		_private->updateTexture(_rc, _texture);

	CharDescriptor desc(value);
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
	_size(size), _face(face), _boldFace(boldFace), _placer(vec2i(defaultTextureSize), true)
{
	_dc = CreateCompatibleDC(0);
	_bitmap = CreateBitmap(defaultTextureSize, defaultTextureSize, 1, 32, 0);
	SelectObject(_dc, _bitmap);

	int pointsSize = -MulDiv(size, GetDeviceCaps(_dc, LOGPIXELSY), 72);

	_font = CreateFont(pointsSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
		CLIP_CHARACTER_PRECIS, CLEARTYPE_NATURAL_QUALITY, FF_DONTCARE, face.c_str());
	_boldFont = CreateFont(pointsSize, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, FF_DONTCARE, boldFace.c_str());

	SetTextColor(_dc, 0xffffff);
	SetBkMode(_dc, TRANSPARENT);
}

CharacterGeneratorPrivate::~CharacterGeneratorPrivate()
{
	DeleteObject(_font);
	DeleteObject(_boldFont);
	DeleteDC(_dc);
}

void CharacterGeneratorPrivate::updateTexture(RenderContext* rc, Texture texture)
{
	BITMAPINFO bm = { sizeof(bm.bmiHeader), defaultTextureSize, defaultTextureSize, 1, 32, 0, sqr(defaultTextureSize) * 4 };

	BinaryDataStorage data(bm.bmiHeader.biSizeImage);
	GetDIBits(_dc, _bitmap, 0, defaultTextureSize, data.data(), &bm, DIB_RGB_COLORS);

	unsigned int* ptr = reinterpret_cast<unsigned int*>(data.data());
	unsigned int* ptrEnd = reinterpret_cast<unsigned int*>(data.data() + data.dataSize());
	while (ptr != ptrEnd)
	{
		unsigned int& value = *ptr++;
		value |= 0x00ffffff | ((((value & 0x000000ff) + ((value & 0x0000ff00) >> 8) + ((value & 0x00ff0000) >> 16) ) / 3) << 24);
	}

	texture->updateDataDirectly(rc, vec2i(defaultTextureSize), data.binary(), data.size());
/*  *
	static int stage = 1;
	char path[256] = { };
	sprintf(path, "d:\\fonts\\%s (%s) - %u - stage %02d.png", _face.c_str(), _boldFace.c_str(), _size, stage++);
	ImageWriter::writeImageToFile(path, data, vec2i(defaultTextureSize), 4, 8, ImageFormat_PNG);
// */
}

void CharacterGeneratorPrivate::renderCharacter(int value, const vec2i& position)
{
	RECT r = { position.x, position.y, defaultTextureSize, defaultTextureSize };
	wchar_t string[2] = { value, 0 };

	SelectObject(_dc, _font);
	DrawTextW(_dc, string, -1, &r, 0);
}

void CharacterGeneratorPrivate::renderBoldCharacter(int value, const vec2i& position)
{
	RECT r = { position.x, position.y, defaultTextureSize, defaultTextureSize };
	wchar_t string[2] = { value, 0 };

	SelectObject(_dc, _boldFont);
	DrawTextW(_dc, string, -1, &r, 0);
}
