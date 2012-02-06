#include <Windows.h>
#include <et/app/application.h>
#include <et/input/input.h>

using namespace et;

PointerInputInfo Input::currentPointer() const
{
	HWND appWindow = HWND(application().renderingContextHandle());
	POINT pt = { };
	RECT r = { };
	GetCursorPos(&pt);
	ScreenToClient(appWindow, &pt);
	GetClientRect(appWindow, &r);
	vec2 ptf(static_cast<float>(pt.x), static_cast<float>(pt.y));

	vec2 normPt;
	normPt.x = 2.0f * ptf.x / static_cast<float>(r.right) - 1.0f;
	normPt.y = 1.0f - 2.0f * ptf.y / static_cast<float>(r.bottom);

	return PointerInputInfo(PointerType_None, ptf, normPt, 0, 0, mainRunLoop()->time());
}


bool Input::canGetCurrentPointerInfo() const
{
	return true;
}