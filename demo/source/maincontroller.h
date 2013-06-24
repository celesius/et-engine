#pragma once

#include <et/app/application.h>
#include "ui/mainmenu.h"
#include "sample/sample.h"

namespace demo
{
	class MainController : public et::IApplicationDelegate
	{
	private:
		et::ApplicationIdentifier applicationIdentifier() const;

		void setRenderContextParameters(et::RenderContextParameters&);

		void applicationDidLoad(et::RenderContext*);
		void applicationWillTerminate();

		void applicationWillResizeContext(const et::vec2i&);
		
		void applicationWillActivate();
		void applicationWillDeactivate();

		void render(et::RenderContext*);
		void idle(float);

	private:
		ResourceManager _resourceManager;

		et::ObjectsCache _mainTextureCache;
		et::gui::Gui::Pointer _gui;
		MainMenuLayout::Pointer _mainMenu;

		Sample _sample;
	};
}
