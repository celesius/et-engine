#include <et/app/application.h>
#include <et/gui/gui.h>
#include <et/scene3d/scene3d.h>
#include <et/timers/inertialvalue.h>
#include <et/input/gestures.h>

namespace fbxc
{
	class Converter : public et::IApplicationDelegate
	{
	public:
		Converter();

	private:
		et::ApplicationIdentifier applicationIdentifier() const;

		void setRenderContextParameters(et::RenderContextParameters&);
		void applicationDidLoad(et::RenderContext*);
		void applicationWillTerminate();

		void render(et::RenderContext*);
		void idle(float);

	private:

		void onPointerPressed(et::PointerInputInfo);
		void onPointerMoved(et::PointerInputInfo);
		void onPointerReleased(et::PointerInputInfo);
		void onZoom(float);
		void onDrag(et::vec2, et::PointerType);
		void onCameraUpdated();

		void onBtnOpenClick(et::gui::Button*); 
		void onBtnSaveClick(et::gui::Button*); 
		void performLoading(std::string);
		void performBinarySaving(std::string);
		void performBinaryWithReadableMaterialsSaving(std::string);

		void renderMeshList(et::RenderContext* rc, const et::s3d::Element::List& meshes);
		void performSceneRendering();

	private:
		et::RenderContext* _rc;
		et::TextureCache _texCache;
		et::GesturesRecognizer _gestures;
		et::AutoPtr<et::gui::Gui> _gui;
		et::s3d::Scene3d _scene;
		et::Program _defaultProgram;
		et::Camera _camera;

		et::gui::Layout::Pointer _mainLayout;
		et::gui::Font _mainFont;
		et::gui::Label::Pointer _labStatus;
		et::gui::Button::Pointer _btnDrawNormalMeshes;
		et::gui::Button::Pointer _btnDrawSupportMeshes;

		et::InertialValue<float> _vDistance;
		et::InertialValue<et::vec2> _vAngle;
	};
}