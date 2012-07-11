#include <et/app/application.h>
#include <et/gui/gui.h>
#include <et/scene3d/scene3d.h>

namespace fbxc
{
	class Converter : public et::IApplicationDelegate, public et::InputHandler
	{
	public:
		Converter();

		void setRenderContextParameters(et::RenderContextParameters&);
		void applicationDidLoad(et::RenderContext*);
		void applicationWillTerminate();

		void render(et::RenderContext*);
		void idle(float);

		void onPointerPressed(et::PointerInputInfo);
		void onPointerMoved(et::PointerInputInfo);
		void onPointerReleased(et::PointerInputInfo);

	private:
		void onBtnOpenClick(et::gui::Button*); 
		void onBtnSaveClick(et::gui::Button*); 
		void performLoading(std::string);
		void performSaving(std::string);

		void renderMeshList(et::RenderContext* rc, const et::s3d::Element::List& meshes);
		void performSceneRendering();

	private:
		et::RenderContext* _rc;
		et::TextureCache _texCache;
		et::AutoPtr<et::gui::Gui> _gui;
		et::s3d::Scene3d _scene;
		et::Program _defaultProgram;
		et::Camera _camera;

		et::gui::Layout::Pointer _mainLayout;
		et::gui::Font _mainFont;
		et::gui::Label::Pointer _labStatus;
		et::gui::Button::Pointer _btnDrawNormalMeshes;
		et::gui::Button::Pointer _btnDrawSupportMeshes;
	};
}