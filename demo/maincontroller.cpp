#include "maincontroller.h"

using namespace et;
using namespace demo;

void MainController::setRenderContextParameters(et::RenderContextParameters&)
{
}

void MainController::applicationDidLoad(et::RenderContext*)
{
}

void MainController::applicationWillTerminate()
{
}

void MainController::applicationWillResizeContext(const et::vec2i&)
{
}

void MainController::applicationWillActivate()
{
}

void MainController::applicationWillDeactivate()
{
}

void MainController::render(et::RenderContext*)
{
}

void MainController::idle(float)
{
}

IApplicationDelegate* et::Application::initApplicationDelegate()
	{ return new MainController; }
