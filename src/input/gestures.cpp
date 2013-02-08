/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/input/gestures.h>

using namespace et;

GesturesRecognizer::GesturesRecognizer(bool automaticMode) : InputHandler(automaticMode),
	_clickThreshold(0.2f), _doubleClickThreshold(0.25f), _holdThreshold(1.0f),
	_singlePointerType(0), _actualTime(0.0f), _clickStartTime(0.0f), _expectClick(false),
	_expectDoubleClick(false), _clickTimeoutActive(false)
{
}

void GesturesRecognizer::handlePointersMovement()
{
    if (_pointers.size() == 2)
    {
        vec2 currentPositions[2];
        vec2 previousPositions[2];
		
        size_t index = 0;
		ET_ITERATE(_pointers, auto, i,
        {
            currentPositions[index] = i.second.current.normalizedPos;
            previousPositions[index] = i.second.previous.normalizedPos;
			++index;
        })
        
        float currentDistance = (currentPositions[0] - currentPositions[1]).length();
        float previousDistance = (previousPositions[0] - previousPositions[1]).length();
        float dz = currentDistance / previousDistance;
        zoom.invoke(dz);
    }
    else 
    {
        
    }
}

void GesturesRecognizer::onPointerPressed(et::PointerInputInfo pi)
{
	pointerPressed.invoke(pi);

	_pointers[pi.id] = PointersInputDelta(pi, pi);
	pressed.invoke(pi.normalizedPos, pi.type);
	
	if (_pointers.size() == 1)
	{
		_singlePointerType = pi.type;
		_singlePointerPosition = pi.normalizedPos;
		float currentTime = mainTimerPool()->actualTime();
		float dt = currentTime - _actualTime;
		
		_clickTimeoutActive = false;
		_expectClick = dt > _doubleClickThreshold;
		_expectDoubleClick = !_expectClick;

		if (_expectClick)
			_clickStartTime = currentTime;
		
		startWaitingForClicks();
	}
	else
	{
		cancelWaitingForClicks();
	}
}

void GesturesRecognizer::onPointerMoved(et::PointerInputInfo pi)
{
	pointerMoved.invoke(pi);
	if (_pointers.count(pi.id) == 0) return;

	_pointers[pi.id].previous = _pointers[pi.id].current;
	_pointers[pi.id].current = pi;

	if (_pointers.size() == 1)
	{
		const PointerInputInfo& pPrev = _pointers[pi.id].previous; 
		const PointerInputInfo& pCurr = _pointers[pi.id].current; 

		moved.invoke(pi.normalizedPos, pi.type);
		drag.invoke((pCurr.normalizedPos - pPrev.normalizedPos) /
			etMax(0.01f, pCurr.timestamp - pPrev.timestamp), pi.type);
	}
	else
	{
		handlePointersMovement();
	}
	
	cancelWaitingForClicks();
}

void GesturesRecognizer::onPointerReleased(et::PointerInputInfo pi)
{
	pointerReleased.invoke(pi);

	_pointers.erase(pi.id);
	released.invoke(pi.normalizedPos, pi.type);
	stopWaitingForClicks();
}

void GesturesRecognizer::onPointerCancelled(et::PointerInputInfo pi)
{
	pointerCancelled.invoke(pi);

	_pointers.erase(pi.id);
	cancelled.invoke();
	cancelWaitingForClicks();
}

void GesturesRecognizer::onPointerScrolled(et::PointerInputInfo i)
{
	scroll.invoke(i.scroll);
}

void GesturesRecognizer::update(float t)
{
	if (_clickTimeoutActive && (t >= _clickStartTime + _clickThreshold))
	{
		click.invoke(_singlePointerPosition, _singlePointerType);
		cancelUpdates();
		_clickTimeoutActive = false;
	}
	
	_actualTime = t;
}

void GesturesRecognizer::startWaitingForClicks()
{
	startUpdates();
}

void GesturesRecognizer::stopWaitingForClicks()
{
	if (_expectDoubleClick)
	{
		doubleClick.invoke(_singlePointerPosition, _singlePointerType);
		_expectDoubleClick = false;
		_expectClick = false;
		cancelUpdates();
	}
	else if (_expectClick)
	{
		_clickTimeoutActive = true;
	}
}

void GesturesRecognizer::cancelWaitingForClicks()
{
	_expectClick = false;
	_expectDoubleClick = false;
	cancelUpdates();
}

void GesturesRecognizer::onGesturePerformed(GestureInputInfo i)
{
	if ((i.mask & GestureTypeMask_Zoom) == GestureTypeMask_Zoom)
		zoom.invoke(1.0f - i.values.z);

	if ((i.mask & GestureTypeMask_Swipe) == GestureTypeMask_Swipe)
		drag.invoke(i.values.xy(), PointerType_None);
}