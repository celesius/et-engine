/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <string>
#include <et/input/input.h>

using namespace et;

Input::Input()
{
	std::memset(_keys, 0, sizeof(_keys));
}

void Input::pushKeyboardInputAction(unsigned char key, InputAction action)
{
	if (action == InputAction_KeyDown)
	{
		_keys[key] = true;
		keyPressed.invokeInMainRunLoop(key);
	} 
	else if (action == InputAction_KeyUp)
	{
		_keys[key] = false;
		keyReleased.invokeInMainRunLoop(key);
	}
	else if (action == InputAction_Char)
	{
		charEntered.invokeInMainRunLoop(key);
	}
}

void Input::pushPointerInputAction(const PointerInputInfo& info, InputAction action)
{
	switch (action)
	{
        case InputAction_PointerPressed:
		{
			addPointerInfo(info);
			pointerPressed.invokeInMainRunLoop(info);
			break;
		}
            
        case InputAction_PointerMoved:
		{
			pointerMoved.invokeInMainRunLoop(info);
			updatePointerInfo(info);
			break;
		}
            
        case InputAction_PointerReleased:
		{
			pointerReleased.invokeInMainRunLoop(info);
			removePointerInfo(info);
			break;
		}
            
        case InputAction_PointerCancelled:
		{
			pointerCancelled.invokeInMainRunLoop(info);
			removePointerInfo(info);
			break;
		}
            
        case InputAction_PointerScrolled:
		{
			pointerScrolled.invokeInMainRunLoop(info);
			break;
		}
            
        default: { }
	}
}

bool Input::isPointerPressed(PointerType type) const
{
	for (PointerInputInfoList::const_iterator i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
	{
		if (i->type == type)
			return true;
	}
    
	return false;
}

void Input::addPointerInfo(const PointerInputInfo& info)
{
	_pointers.push_back(info);
}

void Input::updatePointerInfo(const PointerInputInfo& info)
{
	for (PointerInputInfoList::iterator i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
	{
		if ((i->id == info.id) && (i->type == info.type))
		{
			*i = info;
			break;
		}
	}
}

void Input::removePointerInfo(const PointerInputInfo& info)
{
	for (PointerInputInfoList::iterator i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
	{
		if ((i->id == info.id) && (i->type == info.type))
		{
			_pointers.erase(i);
			break;
		}
	}
}

/*
 * Input Handler
 */

InputHandler::InputHandler()
{
	connectInputEvents();
}

InputHandler::~InputHandler()
{
	input().keyPressed.disconnect(this);
	input().keyReleased.disconnect(this);
	input().charEntered.disconnect(this);
	input().pointerPressed.disconnect(this);
	input().pointerMoved.disconnect(this);
	input().pointerReleased.disconnect(this);
	input().pointerCancelled.disconnect(this);
	input().pointerScrolled.disconnect(this);
}

void InputHandler::connectInputEvents()
{
	input().keyPressed.connect(this, &InputHandler::onKeyPressed);
	input().keyReleased.connect(this, &InputHandler::onKeyReleased);
	input().charEntered.connect(this, &InputHandler::onCharEnterer);
	input().pointerPressed.connect(this, &InputHandler::onPointerPressed);
	input().pointerMoved.connect(this, &InputHandler::onPointerMoved);
	input().pointerReleased.connect(this, &InputHandler::onPointerReleased);
	input().pointerCancelled.connect(this, &InputHandler::onPointerCancelled);
	input().pointerScrolled.connect(this, &InputHandler::onPointerScrolled);
}
