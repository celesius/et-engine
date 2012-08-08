#pragma once

#include <et/app/application.h>

namespace demo
{
	class MainController : public et::IApplicationDelegate
	{
		void setRenderContextParameters(et::RenderContextParameters&);
		void applicationDidLoad(et::RenderContext*);
		void applicationWillTerminate();

		void applicationWillResizeContext(const et::vec2i&);

		void applicationWillActivate();
		void applicationWillDeactivate();

		void render(et::RenderContext*);
		void idle(float);
	};
}
