#include <et/input/gestures.h>

using namespace et;

GesturesRecognizer::GesturesRecognizer() : _scrollZoomScale(0.1f), _doubleClickThreshold(0.3f), _holdThreshold(1.0f)
{
	connectInputEvents();
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
}

void GesturesRecognizer::onPointerReleased(et::PointerInputInfo pi)
{
	_pointers.erase(pi.id);
	released.invoke();
}

void GesturesRecognizer::onPointerCancelled(et::PointerInputInfo pi)
{
	_pointers.erase(pi.id);
	cancelled.invoke();
}

void GesturesRecognizer::onPointerScrolled(et::PointerInputInfo i)
{
	std::cout << i.scroll << std::endl;
	zoom.invoke(1.0f + static_cast<float>(i.scroll) * _scrollZoomScale);
}

