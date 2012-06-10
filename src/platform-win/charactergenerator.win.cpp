#include <Windows.h>
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

		HDC _dc;
		HFONT _font;
		HFONT _boldFont;
		HBITMAP _bitmap;
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
	SIZE characterSize = { };
	wchar_t string[2] = { value, 0 };

	SelectObject(_private->_dc, _private->_font);
	GetTextExtentPointW(_private->_dc, string, 1, &characterSize);

	rect textureRect;
	_private->_placer.place(vec2i(characterSize.cx + 2, characterSize.cy + 2), textureRect);
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
	_dc = CreateCompatibleDC(0);
	_bitmap = CreateBitmap(defaultTextureSize, defaultTextureSize, 1, 32, 0);
	SelectObject(_dc, _bitmap);

	int pointsSize = -MulDiv(size, GetDeviceCaps(_dc, LOGPIXELSY), 72);

	_font = CreateFont(pointsSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
		CLIP_CHARACTER_PRECIS, CLEARTYPE_NATURAL_QUALITY, FF_DONTCARE, face.c_str());
	_boldFont = CreateFont(pointsSize, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, FF_DONTCARE, face.c_str());

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

	for (size_t i = 0; i < data.dataSize() / 4; ++i)
	{
		data[4*i+3] = (76 * data[4*i+0] + 151 * data[4*i+1] + 28 * data[4*i+2]) / 255;
		data[4*i+0] = 255;
		data[4*i+1] = 255;
		data[4*i+2] = 255;
	}

	texture->updateDataDirectly(rc, vec2i(defaultTextureSize), static_cast<char*>(data.raw()), data.size());
/* 
	static int stage = 1;
	char path[256] = { };
	sprintf(path, "d:\\test-%02d.png", stage++);
	ImageWriter::writeImageToFile(path, data, vec2i(defaultTextureSize), 4, 8, ImageFormat_PNG);
// */
}

void CharacterGeneratorPrivate::renderCharacter(int value, const vec2i& position)
{
	RECT r = { position.x, position.y, defaultTextureSize - position.x, defaultTextureSize - position.y };
	wchar_t string[2] = { value, 0 };

	SelectObject(_dc, _font);
	DrawTextW(_dc, string, -1, &r, 0);
}
