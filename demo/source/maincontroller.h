#pragma once

#include <et/app/application.h>
#include "ui/mainmenu.h"

namespace demo
{
	class MainController : public et::IApplicationDelegate
	{
	private:
		et::ApplicationIdentifier applicationIdentifier() const;

		ET_DEPRECATED( void setRenderContextParameters(et::RenderContextParameters&) );

		void applicationDidLoad(et::RenderContext*) __attribute__((deprecated));
		void applicationWillTerminate();

		void applicationWillResizeContext(const et::vec2i&);
		
		void applicationWillActivate();
		void applicationWillDeactivate();

		void render(et::RenderContext*);
		void idle(float);

	private:
		ResourceManager _resourceManager;

		et::TextureCache _mainTextureCache;
		et::gui::Gui::Pointer _gui;
		MainMenuLayout::Pointer _mainMenu;
	};
}
