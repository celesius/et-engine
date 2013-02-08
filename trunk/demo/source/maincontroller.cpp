#include "maincontroller.h"

using namespace et;
using namespace demo;

void MainController::setRenderContextParameters(et::RenderContextParameters& p)
{
	p.supportedInterfaceOrientations =
		InterfaceOrientation_Any & (~InterfaceOrientation_PortraitUpsideDown);

#if (ET_PLATFORM_IOS)
	p.baseContextSize = vec2i(320, 480);
#else
	p.baseContextSize = vec2i(800, 600);
	p.contextSize = p.baseContextSize;
#endif
}

void MainController::applicationDidLoad(et::RenderContext* rc)
{
	_resourceManager.load(rc);
	_gui = gui::Gui::Pointer(new gui::Gui(rc));

	_mainMenu = MainMenuLayout::Pointer(new MainMenuLayout(rc, _resourceManager));
	_gui->pushLayout(_mainMenu);
}

void MainController::applicationWillTerminate()
{
	
}

void MainController::applicationWillResizeContext(const et::vec2i& sz)
{
	vec2 fsz(static_cast<float>(sz.x), static_cast<float>(sz.y));
	_gui->layout(fsz);
}

void MainController::applicationWillActivate()
{

}

void MainController::applicationWillDeactivate()
{
	
}

void MainController::render(et::RenderContext* rc)
{
	rc->renderer()->clear();
	_gui->render(rc);
}

void MainController::idle(float)
{
	
}

ApplicationIdentifier MainController::applicationIdentifier() const
	{ return ApplicationIdentifier(applicationIdentifierForCurrentProject(), "Cheetek", "Test"); }

IApplicationDelegate* Application::initApplicationDelegate()
	{ return new MainController; }
