#include <et/input/input.h>

using namespace et;

PointerInputInfo Input::currentPointer() const
{
	return PointerInputInfo(PointerType_None, vec2(0.0f), vec2(0.0f), 0, 0, queryTime());
}

bool Input::canGetCurrentPointerInfo() const
{
	return false;
}