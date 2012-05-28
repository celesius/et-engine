/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/input/input.h>
#include <et/gui/guirenderer.h>
#include <et/gui/keyboard.h>

using namespace et;
using namespace et::gui;

const char* primaryButtons[] = 
{
	"1234567890",
	"qwertyuiop",
	"asdfghjkl",
	"zxcvbnm",
};

const char* extraButtons[] = 
{
	"~#$%^&*\\|",
	"()[]{}<>",
	"/:;+='`\"",
	".,-@_!?",
};

bool supportedKey(unsigned char c)
{
	unsigned char lowCaseChar = static_cast<unsigned char>(tolower(c));
	size_t rowCount = sizeof(primaryButtons) / sizeof(const char*);
	for (size_t i = 0; i < rowCount; ++i)
	{
		size_t charCount = strlen(primaryButtons[i]);
		for (size_t j = 0; j < charCount; ++j)
		{
			if (primaryButtons[i][j] == lowCaseChar)
				return true;
		}
	}

	rowCount = sizeof(extraButtons) / sizeof(const char*);
	for (size_t i = 0; i < rowCount; ++i)
	{
		size_t charCount = strlen(extraButtons[i]);
		for (size_t j = 0; j < charCount; ++j)
		{
			if (extraButtons[i][j] == lowCaseChar)
				return true;
		}
	}

	return false;
}

enum ServiceButton
{
	ServiceButton_Shift,
	ServiceButton_Delete,
	ServiceButton_Punctuation,
	ServiceButton_Space,
	ServiceButton_Return,
	ServiceButton_PrimaryButtons,
	ServiceButton_max
};

const char* serviceButtonsNames[ServiceButton_max] = 
{
	"Shift", "Delete", "()/+#", "Space", "Enter", "ABC"
};

const float buttonOffset = 1.0f;
const float buttonStartOffset = 5.0f;
const int numKeyboardRows = 4;
const int maxCharsPerRow = 10;
const float yWhitespaceSize = 2.0f * buttonStartOffset + buttonOffset * static_cast<float>(numKeyboardRows);
const float switchKeyboardDuration = 0.15f;
const float showHideDuration = 0.3f;

Keyboard::Keyboard(RenderContext* rc, Font font, TextureCache& cache) : Element2D(0), _delegate(0),
	_descriptor(vec2(0.0f), vec2(128.0f), ContentOffset(6.0f)), _lastActiveButton(0), _pressedButton(0), _capturedElement(0),
	_extraButtonsVisible(false), _visible(false), _shift(false), _shiftLocked(false)
{
	_texture = rc->textureFactory().loadTexture("ui/keyboard", cache);
	
	setPivotPoint(vec2(0.5f, 0.0f));
	layout(rc->size());
	
	genExtraButtons(font);
	float lastRowWidth = genPrimaryButtons(font);
	genServiceButtons(font, lastRowWidth);

	input().keyPressed.connect(this, &Keyboard::onKeyPressed);
	input().keyReleased.connect(this, &Keyboard::onKeyReleased);
	input().charEntered.connect(this, &Keyboard::onCharEntered);
}

Keyboard::~Keyboard()
{
}

void Keyboard::layout(const vec2& sz)
{
	_screenSize = sz;
	setFrame(floorv(vec2(0.5f * sz.x, sz.y * 1.5f + 1.0f)), vec2(sz.x, 0.5f * sz.y));
	setInvalid();
}

Button::Pointer Keyboard::createButton(ButtonList& list, const std::string& title, Font f, bool accented, const vec2& pivot)
{
	Button::Pointer b = Button::Pointer(new Button(title, f, this));
	b->setPivotPoint(pivot);
	list.push_back(b);

	if (accented)
	{
		Image imgAccentDefaultState(_texture, ImageDescriptor(vec2(128.0f, 32.0f), vec2(32.0f), ContentOffset(8.0f)));
		Image imgAccentHoveredState(_texture, ImageDescriptor(vec2(160.0f, 32.0f), vec2(32.0f), ContentOffset(8.0f)));
		Image imgAccentPressedState(_texture, ImageDescriptor(vec2(192.0f, 32.0f), vec2(32.0f), ContentOffset(8.0f)));
		b->setBackgroundForState(imgAccentDefaultState, ElementState_Default);
		b->setBackgroundForState(imgAccentHoveredState, ElementState_Hovered);
		b->setBackgroundForState(imgAccentPressedState, ElementState_Pressed);
		b->setTextColor(vec4(1.0f));
	}
	else
	{
		Image imgDefaultState(_texture, ImageDescriptor(vec2(128.0f, 0.0f), vec2(32.0f), ContentOffset(6.0f)));
		Image imgHoveredState(_texture, ImageDescriptor(vec2(160.0f, 0.0f), vec2(32.0f), ContentOffset(6.0f)));
		Image imgPressedState(_texture, ImageDescriptor(vec2(192.0f, 0.0f), vec2(32.0f), ContentOffset(6.0f)));
		b->setBackgroundForState(imgDefaultState, ElementState_Default);
		b->setBackgroundForState(imgHoveredState, ElementState_Hovered);
		b->setBackgroundForState(imgPressedState, ElementState_Pressed);
		b->setTextColor(vec4(0.1f, 0.1f, 0.1f, 1.0f));
	}

	return b;
}

void Keyboard::genServiceButtons(Font font, float lastRowWidth)
{
	float buttonHeight = floorf((size().y - yWhitespaceSize) / static_cast<float>(numKeyboardRows + 1));
	float lastRowY = buttonStartOffset + static_cast<float>(numKeyboardRows - 1) * buttonHeight + static_cast<float>(numKeyboardRows - 1) * buttonOffset;
	float serviceButtonsWidth = 2.0f * buttonStartOffset;
	float serviceButtonExtraSize = 51.0f;

	Button::Pointer b;

	vec2 textSize = floorv(font->measureStringSize(serviceButtonsNames[ServiceButton_Shift]) + vec2(serviceButtonExtraSize, 0.0f));
	b = createButton(_serviceButtons, serviceButtonsNames[ServiceButton_Shift], font, true);
	b->setFrame(buttonStartOffset, lastRowY, 0.5f * (size().x - lastRowWidth) - buttonOffset, buttonHeight);
	b->clicked.connect(this, &Keyboard::onShiftPressed);

	b = createButton(_serviceButtons, serviceButtonsNames[ServiceButton_Delete], font, true, vec2(1.0f, 0.0f));
	b->setFrame(size().x - buttonStartOffset, lastRowY, 0.5f * (size().x - lastRowWidth) - buttonOffset, buttonHeight);
	b->clicked.connect(this, &Keyboard::onDeletePressed);

	textSize = floorv(font->measureStringSize(serviceButtonsNames[ServiceButton_Punctuation]) + vec2(serviceButtonExtraSize, 0.0f));
	b = createButton(_serviceButtons, serviceButtonsNames[ServiceButton_Punctuation], font, true, vec2(0.0f, 1.0f));
	b->setFrame(buttonStartOffset, size().y - buttonStartOffset, textSize.x, buttonHeight);
	b->clicked.connect(this, &Keyboard::onSwitchKeyboardsPressed);
	serviceButtonsWidth += textSize.x;

	textSize = floorv(font->measureStringSize(serviceButtonsNames[ServiceButton_Return]) + vec2(serviceButtonExtraSize, 0.0f));
	serviceButtonsWidth += textSize.x;

	b = createButton(_serviceButtons, serviceButtonsNames[ServiceButton_Space], font, false, vec2(0.5f, 1.0f));
	b->setFrame(floorf(0.5f * size().x), floorf(size().y - buttonStartOffset), 
		floorf(size().x - serviceButtonsWidth - serviceButtonExtraSize), buttonHeight);
	b->clicked.connect(this, &Keyboard::onSpacePressed);

	b = createButton(_serviceButtons, serviceButtonsNames[ServiceButton_Return], font, true, vec2(1.0f, 1.0f));
	b->setFrame(size().x - buttonStartOffset, floorf(size().y - buttonStartOffset), textSize.x, buttonHeight);
	b->clicked.connect(this, &Keyboard::onReturnPressed);
}

float Keyboard::genPrimaryButtons(Font font)
{
	float buttonHeight = floorf((size().y - yWhitespaceSize) / static_cast<float>(numKeyboardRows + 1));
	float yOffset = buttonStartOffset;
	float dy = buttonHeight + buttonOffset;
	float rowWidth = 0.0f;
	for (int i = 0; i < numKeyboardRows; ++i)
	{
		int numChars = strlen(primaryButtons[i]);
		float fNumChars = static_cast<float>((numChars > maxCharsPerRow) ? maxCharsPerRow : numChars);
		float xWhitespaceSize = 2.0f * buttonStartOffset + buttonOffset * (fNumChars - 1.0f);
		float buttonWidth = floorf((size().x - xWhitespaceSize) / static_cast<float>(maxCharsPerRow));
		rowWidth = fNumChars * buttonWidth + xWhitespaceSize;
		float xOffset = floorf(buttonStartOffset + 0.5f * (size().x - rowWidth));
		for (int j = 0; j < numChars; ++j)
		{
			char title[2] = {primaryButtons[i][j], 0};
			Button::Pointer b = createButton(_primaryButtons, title, font, false);
			b->setFrame(xOffset, yOffset, buttonWidth, buttonHeight);
			b->clicked.connect(this, &Keyboard::onButtonPressed);
			xOffset += buttonWidth + buttonOffset;
		}

		yOffset += dy;
	}

	return rowWidth;
}

float Keyboard::genExtraButtons(Font font)
{
	float buttonHeight = floorf((size().y - yWhitespaceSize) / static_cast<float>(numKeyboardRows + 1));
	float yOffset = buttonStartOffset;
	float dy = buttonHeight + buttonOffset;
	float rowWidth = 0.0f;
	for (int i = 0; i < numKeyboardRows; ++i)
	{
		int numChars = strlen(extraButtons[i]);
		float fNumChars = static_cast<float>((numChars > maxCharsPerRow) ? maxCharsPerRow : numChars);
		float xWhitespaceSize = 2.0f * buttonStartOffset + buttonOffset * (fNumChars - 1.0f);
		float buttonWidth = floorf((size().x - xWhitespaceSize) / static_cast<float>(maxCharsPerRow));
		rowWidth = fNumChars * buttonWidth + xWhitespaceSize;
		float xOffset = floorf(buttonStartOffset + 0.5f * (size().x - rowWidth));
		for (int j = 0; j < numChars; ++j)
		{
			char title[2] = {extraButtons[i][j], 0};
			Button::Pointer b = createButton(_extraButtons, title, font, false);
			b->setAlpha(0.0f);
			b->setFrame(xOffset, yOffset, buttonWidth, buttonHeight);
			b->clicked.connect(this, &Keyboard::onButtonPressed);
			xOffset += buttonWidth + buttonOffset;
		}

		yOffset += dy;
	}

	return rowWidth;
}

void Keyboard::addToRenderQueue(RenderContext* rc, GuiRenderer& gr)
{
	if (platformHasHardwareKeyboard()) return;

	if (!contentValid())
	{
		_vertices.setOffset(0);
		gr.createImageVertices(_vertices, _texture,	_descriptor, rect(vec2(0.0f), size()), vec4(1.0f), finalTransform(), GuiRenderLayer_Layer0);
		setContentValid();
	}
	gr.addVertices(_vertices, _texture, ElementClass_2d, GuiRenderLayer_Layer0);

	ButtonList::iterator i = _extraButtonsVisible ? _extraButtons.begin() : _primaryButtons.begin();
	ButtonList::iterator e = _extraButtonsVisible ? _extraButtons.end() : _primaryButtons.end();

	for (; i != e; ++i)
		(*i)->addToRenderQueue(rc, gr);

	for (i = _serviceButtons.begin(), e = _serviceButtons.end(); i != e; ++i)
		(*i)->addToRenderQueue(rc, gr);

	_invalid = false;
}

void Keyboard::switchKeyboards(bool animated)
{
	float duration = animated ? switchKeyboardDuration : 0.0f;

	if (_extraButtonsVisible)
	{
		_serviceButtons.at(ServiceButton_Punctuation)->setTitle(serviceButtonsNames[ServiceButton_Punctuation]);
		_serviceButtons.at(ServiceButton_Shift)->setVisible(true, duration);
	}
	else
	{
		_serviceButtons.at(ServiceButton_Punctuation)->setTitle(serviceButtonsNames[ServiceButton_PrimaryButtons]);
		_serviceButtons.at(ServiceButton_Shift)->setVisible(false, duration);
	}

	for (ButtonList::iterator i = _primaryButtons.begin(), e = _primaryButtons.end(); i != e; ++i)
		(*i)->setVisible(_extraButtonsVisible, duration);

	for (ButtonList::iterator i = _extraButtons.begin(), e = _extraButtons.end(); i != e; ++i)
		(*i)->setVisible(!_extraButtonsVisible, duration);

	_extraButtonsVisible = !_extraButtonsVisible;
	_invalid = true;
}

bool Keyboard::pointerPressed(const PointerInputInfo& p)
{
	_pressedButton.reset(0);

	ButtonList::iterator i = _extraButtonsVisible ? _extraButtons.begin() : _primaryButtons.begin();
	ButtonList::iterator e = _extraButtonsVisible ? _extraButtons.end() : _primaryButtons.end();
	for (; i != e; ++i)
	{
		Button::Pointer btn = *i;
		if (btn->containsPoint(p.pos, p.normalizedPos))
		{
			_pressedButton = btn;
			btn->pointerPressed(PointerInputInfo(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp));
			return true;
		}
	}

	for (i = _serviceButtons.begin(), e = _serviceButtons.end(); i != e; ++i)
	{
		Button::Pointer btn = *i;
		if (btn->containsPoint(p.pos, p.normalizedPos))
		{
			_pressedButton = btn;
			btn->pointerPressed(PointerInputInfo(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp));
			return true;
		}
	}

	return containsPoint(p.pos, p.normalizedPos);
}

void Keyboard::setActiveButton(Button::Pointer btn, const PointerInputInfo& np)
{
	if (btn == _lastActiveButton) return;

	if (_lastActiveButton.valid())
		_lastActiveButton->pointerLeaved(np);

	_lastActiveButton = btn;

	if (_lastActiveButton.valid())
		_lastActiveButton->pointerEntered(np);
}

bool Keyboard::pointerMoved(const PointerInputInfo& p)
{
	ButtonList::iterator i = _extraButtonsVisible ? _extraButtons.begin() : _primaryButtons.begin();
	ButtonList::iterator e = _extraButtonsVisible ? _extraButtons.end() : _primaryButtons.end();
	for (; i != e; ++i)
	{
		Button::Pointer btn = *i;
		if (btn->containsPoint(p.pos, p.normalizedPos))
		{
			PointerInputInfo np(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp);
			setActiveButton(btn, np);
			btn->pointerMoved(np);
			return true;
		}
	}

	for (i = _serviceButtons.begin(), e = _serviceButtons.end(); i != e; ++i)
	{
		Button::Pointer btn = *i;
		if (btn->containsPoint(p.pos, p.normalizedPos))
		{
			PointerInputInfo np(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp);
			setActiveButton(btn, np);
			btn->pointerMoved(np);
			return true;
		}
	}

	setActiveButton(Button::Pointer(), p);
	return containsPoint(p.pos, p.normalizedPos);
}

bool Keyboard::pointerReleased(const PointerInputInfo& p)
{
	bool processed = false;

	ButtonList::iterator i = _extraButtonsVisible ? _extraButtons.begin() : _primaryButtons.begin();
	ButtonList::iterator e = _extraButtonsVisible ? _extraButtons.end() : _primaryButtons.end();
	for (; i != e; ++i)
	{
		Button::Pointer btn = *i;
		if (btn->containsPoint(p.pos, p.normalizedPos))
		{
			btn->pointerReleased(PointerInputInfo(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp));
			processed = true;
			break;
		}
	}

	if (!processed)
	{
		for (i = _serviceButtons.begin(), e = _serviceButtons.end(); i != e; ++i)
		{
			Button::Pointer btn = *i;
			if (btn->containsPoint(p.pos, p.normalizedPos))
			{
				btn->pointerReleased(PointerInputInfo(p.type, btn->positionInElement(p.pos), p.normalizedPos, p.scroll, p.id, p.timestamp));
				processed = true;
				break;
			}
		}
	}

	if (_pressedButton.valid())
	{
		_pressedButton->pointerReleased(p);
		_pressedButton.reset(0);
	}

	return processed || containsPoint(p.pos, p.normalizedPos);
}

bool Keyboard::pointerScrolled(const PointerInputInfo& p)
{
	return containsPoint(p.pos, p.normalizedPos);
}

void Keyboard::onButtonPressed(Button* b)
{
	performButtonAction(b->title()[0]);

	if (_shift)
		onShiftPressed(0);
}

void Keyboard::onShiftPressed(Button*)
{
	if (_shiftLocked)
		_shift = true;
	else
		_shift = !_shift;

	for (ButtonList::iterator i = _primaryButtons.begin(), e = _primaryButtons.end(); i != e; ++i)
	{
		char text[2] = { (*i)->title()[0], 0 };
		text[0] = static_cast<char>(_shift ? toupper(text[0]) : tolower(text[0]));
		(*i)->setTitle(text);
	}
}

void Keyboard::onSpacePressed(Button*)
{
	performButtonAction(ET_SPACE);
}

void Keyboard::onReturnPressed(Button*)
{
	if (_delegate)
		_delegate->keyboardDidReturnInElement(this, _capturedElement);
}

void Keyboard::onDeletePressed(Button*)
{
	performButtonAction(ET_BACKSPACE);
}

void Keyboard::onSwitchKeyboardsPressed(Button*)
{
	switchKeyboards(true);
}

void Keyboard::setInvalid()
{
	_invalid = true;
}

void Keyboard::show(bool animated, Element* forElement)
{
	_capturedElement = forElement;
	if (_visible) return;

	if (animated)
		setPosition(0.5f * _screenSize.x, _screenSize.y * 1.5f + 1.0f);

	setPosition(0.5f * _screenSize.x, topOrigin(), animated ? showHideDuration : 0.0f);
	_visible = true;
}

void Keyboard::hide(bool animated)
{
	_capturedElement = 0;
	if (!_visible) return;

	_hideAnimator = setPosition(0.5f * _screenSize.x, _screenSize.y * 1.5f + 1.0f, animated ? showHideDuration : 0.0f);
}

void Keyboard::animatorFinished(BaseAnimator* animator)
{
	if (animator == _hideAnimator)
		_visible = false;

	Element2D::animatorFinished(animator);
}

void Keyboard::setDelegate(KeyboardDelegate* delegate)
{
	_delegate = delegate;
}

void Keyboard::onKeyPressed(unsigned char c)
{
	if (!_visible) return;

	if (c == ET_SHIFT)
	{
		_shiftLocked = true;
		onShiftPressed(0);
		_shift = true;
	}
	else if (c == ET_BACKSPACE)
	{
		onDeletePressed(0);
	}
	else if (c == ET_RETURN)
	{
		onReturnPressed(0);
	}
}

void Keyboard::onKeyReleased(unsigned char c)
{
	if (c == ET_SHIFT)
	{
		_shift = true;
		_shiftLocked = false;
		onShiftPressed(0);
	}
}

void Keyboard::performButtonAction(unsigned char c)
{
	if (_capturedElement)
		_capturedElement->processMessage(GuiMessage(GuiMessage::Type_TextInput, c));
}

void Keyboard::onCharEntered(unsigned char c)
{
	if (!_visible) return;

	if ((c == ET_SPACE) || supportedKey(c))
		performButtonAction(_shift ? c : static_cast<unsigned char>(tolower(c)));
}

float Keyboard::topOrigin() const
{
	return (platformHasHardwareKeyboard() ? 1.0f : 0.5f) * _screenSize.y;
}