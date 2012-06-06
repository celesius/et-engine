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
	_scrollZoomScale(0.1f), _clickThreshold(0.25f), _doubleClickThreshold(0.25f), _holdThreshold(1.0f), _actualTime(0.0f),
	_clickStartTime(0.0f), _expectClick(false), _expectDoubleClick(false), _clickTimeoutActive(false)
{
}

void GesturesRecognizer::handlePointersMovement()
{
    if (_pointers.size() == 2)
    {
        vec2 currentPositions[2];
        vec2 previousPositions[2];
        size_t index = 0;
        for (PointersInputDeltaMap::iterator i = _pointers.begin(), e = _pointers.end(); i != e; ++i, ++index)
        {
            currentPositions[index] = i->second.current.normalizedPos;
            previousPositions[index] = i->second.previous.normalizedPos;
        }
        
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
	_pointers[pi.id] = PointersInputDelta(pi, pi);
	pressed.invoke();
	
	if (_pointers.size() == 1)
	{
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
	if (_pointers.size() == 0) return;

	_pointers[pi.id].previous = _pointers[pi.id].current;
	_pointers[pi.id].current = pi;

	if (_pointers.size() == 1)
	{
		const PointerInputInfo& pPrev = _pointers[pi.id].previous; 
		const PointerInputInfo& pCurr = _pointers[pi.id].current; 
		vec2 dp = pCurr.normalizedPos - pPrev.normalizedPos;
		float dt = etMax(0.01f, pCurr.timestamp - pPrev.timestamp);
		drag.invoke(dp / dt);
	}
	else
	{
		handlePointersMovement();
	}
	
	cancelWaitingForClicks();
}

void GesturesRecognizer::onPointerReleased(et::PointerInputInfo pi)
{
	_pointers.erase(pi.id);
	released.invoke();
	
	stopWaitingForClicks();
}

void GesturesRecognizer::onPointerCancelled(et::PointerInputInfo pi)
{
	_pointers.erase(pi.id);
	cancelled.invoke();
	
	cancelWaitingForClicks();
}

void GesturesRecognizer::onPointerScrolled(et::PointerInputInfo i)
{
	std::cout << i.scroll << std::endl;
	zoom.invoke(1.0f + static_cast<float>(i.scroll) * _scrollZoomScale);
}

void GesturesRecognizer::update(float t)
{
	if (_clickTimeoutActive && (t >= _clickStartTime + _clickThreshold))
	{
		click.invoke();
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
		doubleClick.invoke();
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
