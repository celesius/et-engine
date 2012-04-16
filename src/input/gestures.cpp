#include <et/input/gestures.h>

using namespace et;

GesturesRecognizer::GesturesRecognizer() : _scrollZoomScale(0.1f), _doubleClickThreshold(0.3f), _holdThreshold(1.0f)
{
	connectInputEvents();
}

void GesturesRecognizer::handlePointersMovement()
{

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

void GesturesRecognizer::onPointerScrolled(et::PointerInputInfo i)
{
	std::cout << i.scroll << std::endl;
	zoom.invoke(1.0f + static_cast<float>(i.scroll) * _scrollZoomScale);
}

