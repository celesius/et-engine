/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/input/input.h>

using namespace et;

Input::Input()
{
}

void Input::pushKeyboardInputAction(size_t key, InputAction action)
{
	if (action == InputAction_KeyDown)
	{
		_pressedKeys[key] = 1;
		keyPressed.invokeInMainRunLoop(key);
	} 
	else if (action == InputAction_KeyUp)
	{
		_pressedKeys.erase(key);
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
	for (auto i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
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
	for (auto i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
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
	for (auto i = _pointers.begin(), e = _pointers.end(); i != e; ++i)
	{
		if ((i->id == info.id) && (i->type == info.type))
		{
			_pointers.erase(i);
			break;
		}
	}
}

void Input::pushGestureInputAction(const et::GestureInputInfo& g)
{
	gesturePerformed.invokeInMainRunLoop(g);
}

/*
 * Input Handler
 */

InputHandler::InputHandler(bool connect)
{
	if (connect)
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
	ET_CONNECT_EVENT(input().keyPressed, InputHandler::onKeyPressed)
	ET_CONNECT_EVENT(input().keyReleased, InputHandler::onKeyReleased)
	ET_CONNECT_EVENT(input().charEntered, InputHandler::onCharEnterer)
	ET_CONNECT_EVENT(input().pointerPressed, InputHandler::onPointerPressed)
	ET_CONNECT_EVENT(input().pointerMoved, InputHandler::onPointerMoved)
	ET_CONNECT_EVENT(input().pointerReleased, InputHandler::onPointerReleased)
	ET_CONNECT_EVENT(input().pointerCancelled, InputHandler::onPointerCancelled)
	ET_CONNECT_EVENT(input().pointerScrolled, InputHandler::onPointerScrolled)
	ET_CONNECT_EVENT(input().gesturePerformed, InputHandler::onGesturePerformed)
}

/*
 * Support functions
 */
std::ostream& et::operator << (std::ostream& s, const PointerInputInfo& p)
{
	s << "Pointer {" << p.type << ", pos: " << p.pos << "}" << std::endl;
	return s;
}