#include <Windows.h>
#include <et/models/fbxloader.h>

#include "converter.h"

using namespace fbxc;
using namespace et;

Converter::Converter() : _rc(0)
{
}

void Converter::setRenderContextParameters(RenderContextParameters& p)
{
	p.contextSize = vec2i(1024, 768);
}

void Converter::applicationDidLoad(RenderContext* rc)
{
	_rc = rc;
	_rc->renderState().setDepthTest(true);

	_gui = new gui::Gui(rc, _texCache);
	_mainLayout = gui::Layout::Pointer(new gui::Layout());
	_gui->pushLayout(_mainLayout, 0, 0.0f);
	_mainFont = gui::Font(rc, "ui/fonts/main.font", _texCache);

	Texture uiTexture = rc->textureFactory().loadTexture("gui/keyboard.png", _texCache);

	gui::Image imgNormalState(uiTexture, gui::ImageDescriptor(vec2(128.0f, 0.0f), vec2(32.0f), gui::ContentOffset(8.0f)));
	gui::Image imgHoverState(uiTexture, gui::ImageDescriptor(vec2(160.0f, 0.0f), vec2(32.0f), gui::ContentOffset(8.0f)));
	gui::Image imgPressedState(uiTexture, gui::ImageDescriptor(vec2(192.0f, 0.0f), vec2(32.0f), gui::ContentOffset(8.0f)));
	gui::Image imgSelectedNormalState(uiTexture, gui::ImageDescriptor(vec2(128.0f, 32.0f), vec2(32.0f), gui::ContentOffset(8.0f)));
	gui::Image imgSelectedHoverState(uiTexture, gui::ImageDescriptor(vec2(160.0f, 32.0f), vec2(32.0f), gui::ContentOffset(8.0f)));
	gui::Image imgSelectedPressedState(uiTexture, gui::ImageDescriptor(vec2(192.0f, 32.0f), vec2(32.0f), gui::ContentOffset(8.0f)));

	gui::Button::Pointer btnOpen(new gui::Button("Open", _mainFont, _mainLayout.ptr()));
	btnOpen->setBackgroundForState(imgNormalState, gui::ElementState_Default);
	btnOpen->setBackgroundForState(imgHoverState, gui::ElementState_Hovered);
	btnOpen->setBackgroundForState(imgPressedState, gui::ElementState_Pressed);
	btnOpen->clicked.connect(this, &Converter::onBtnOpenClick);

	gui::Button::Pointer btnSave(new gui::Button("Save", _mainFont, _mainLayout.ptr()));
	btnSave->setBackgroundForState(imgNormalState, gui::ElementState_Default);
	btnSave->setBackgroundForState(imgHoverState, gui::ElementState_Hovered);
	btnSave->setBackgroundForState(imgPressedState, gui::ElementState_Pressed);
	btnSave->setPosition(btnOpen->size().x, 0.0f);
	btnSave->clicked.connect(this, &Converter::onBtnSaveClick);

	_btnDrawNormalMeshes = gui::Button::Pointer(new gui::Button("Normal", _mainFont, _mainLayout.ptr()));
	_btnDrawNormalMeshes->setBackgroundForState(imgNormalState, gui::ElementState_Default);
	_btnDrawNormalMeshes->setBackgroundForState(imgHoverState, gui::ElementState_Hovered);
	_btnDrawNormalMeshes->setBackgroundForState(imgPressedState, gui::ElementState_Pressed);
	_btnDrawNormalMeshes->setBackgroundForState(imgSelectedNormalState, gui::ElementState_Selected);
	_btnDrawNormalMeshes->setBackgroundForState(imgSelectedHoverState, gui::ElementState_SelectedHovered);
	_btnDrawNormalMeshes->setBackgroundForState(imgSelectedPressedState, gui::ElementState_SelectedPressed);
	_btnDrawNormalMeshes->setPivotPoint(vec2(1.0f));
	_btnDrawNormalMeshes->setPosition(_rc->size());
	_btnDrawNormalMeshes->setType(gui::Button::Type_CheckButton);
	_btnDrawNormalMeshes->setSelected(true);

	_btnDrawSupportMeshes = gui::Button::Pointer(new gui::Button("Support", _mainFont, _mainLayout.ptr()));
	_btnDrawSupportMeshes->setBackgroundForState(imgNormalState, gui::ElementState_Default);
	_btnDrawSupportMeshes->setBackgroundForState(imgHoverState, gui::ElementState_Hovered);
	_btnDrawSupportMeshes->setBackgroundForState(imgPressedState, gui::ElementState_Pressed);
	_btnDrawSupportMeshes->setBackgroundForState(imgSelectedNormalState, gui::ElementState_Selected);
	_btnDrawSupportMeshes->setBackgroundForState(imgSelectedHoverState, gui::ElementState_SelectedHovered);
	_btnDrawSupportMeshes->setBackgroundForState(imgSelectedPressedState, gui::ElementState_SelectedPressed);
	_btnDrawSupportMeshes->setPivotPoint(vec2(1.0f));
	_btnDrawSupportMeshes->setPosition(_rc->size() - vec2(_btnDrawNormalMeshes->size().x, 0.0f));
	_btnDrawSupportMeshes->setType(gui::Button::Type_CheckButton);
	_btnDrawSupportMeshes->setSelected(true);

	_labStatus = gui::Label::Pointer(new gui::Label("Status", _mainFont, _mainLayout.ptr()));
	_labStatus->setPivotPoint(vec2(0.0f, 1.0f));
	_labStatus->setPosition(0.0f, _rc->size().y);
	_labStatus->setText("Ready.");

	_camera.perspectiveProjection(QUARTER_PI, rc->size().aspect(), 1.0f, 2000.0f);
	_camera.lookAt(200.0f * vec3(1.0f, 1.0f, 1.0f));

	_defaultProgram = rc->programFactory().loadProgram("default.program");
	_defaultProgram->setPrimaryLightPosition(500.0f * vec3(0.0f, 1.0f, 0.0f));
	_defaultProgram->setUniform("diffuseMap", 0);
	_defaultProgram->setUniform("specularMap", 1);
	_defaultProgram->setUniform("normalMap", 2);

	const std::string& lp = application().launchParameter(1);
	if (fileExists(lp))
	{
		Invocation1 i;
		i.setTarget(this, &Converter::performLoading, lp);
		i.invokeInMainRunLoop();
	}
}

void Converter::applicationWillTerminate()
{
}

void Converter::renderMeshList(RenderContext* rc, const s3d::Element::List& meshes)
{
	for (s3d::Element::List::const_iterator i = meshes.begin(), e = meshes.end(); i != e; ++i)
	{
		s3d::Mesh::Pointer mesh = *i;
		if (mesh->active())
		{
			Material& m = mesh->material();
			_defaultProgram->setUniform("ambientColor", m->getVec4(MaterialParameter_AmbientColor));
			_defaultProgram->setUniform("diffuseColor", m->getVec4(MaterialParameter_DiffuseColor));
			_defaultProgram->setUniform("specularColor", m->getVec4(MaterialParameter_SpecularColor));
			_defaultProgram->setUniform("roughness", m->getFloat(MaterialParameter_Roughness));
			_defaultProgram->setUniform("mTransform", mesh->finalTransform());
			rc->renderState().bindTexture(0, mesh->material()->getTexture(MaterialParameter_DiffuseMap));
			rc->renderState().bindTexture(1, mesh->material()->getTexture(MaterialParameter_SpecularMap));
			rc->renderState().bindTexture(2, mesh->material()->getTexture(MaterialParameter_NormalMap));
			rc->renderState().bindVertexArray(mesh->vertexArrayObject());
			rc->renderer()->drawElements(mesh->indexBuffer(), mesh->startIndex(), mesh->numIndexes());
		}
	}
}

void Converter::performSceneRendering()
{
	_rc->renderState().bindProgram(_defaultProgram);
	_defaultProgram->setCameraProperties(_camera);

	if (_btnDrawNormalMeshes->selected())
		renderMeshList(_rc, _scene.childrenOfType(s3d::ElementType_Mesh));

	if (_btnDrawSupportMeshes->selected())
		renderMeshList(_rc, _scene.childrenOfType(s3d::ElementType_SupportMesh));
}

void Converter::render(RenderContext* rc)
{
	rc->renderer()->clear();
	performSceneRendering();

	rc->renderer()->clear(false, true);
	_gui->render(rc);
}

void Converter::idle(float)
{

}

void Converter::onPointerPressed(et::PointerInputInfo p)
{
	_gui->pointerPressed(p);
}

void Converter::onPointerMoved(et::PointerInputInfo p)
{
	_gui->pointerMoved(p);
}

void Converter::onPointerReleased(et::PointerInputInfo p)
{
	_gui->pointerReleased(p);
}

void Converter::onBtnOpenClick(et::gui::Button*)
{
	char filename[MAX_PATH] = { };

	OPENFILENAME of = { };
	of.lStructSize = sizeof(of);
	of.hwndOwner = reinterpret_cast<HWND>(application().renderingContextHandle());
	of.hInstance = GetModuleHandle(0);
	of.lpstrFilter = "All supported files\0*.fbx;*.etm\0FBX files\0*.fbx\0ET models\0*.etm\0\0";
	of.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	of.lpstrFile = filename;
	of.nMaxFile = MAX_PATH;

	if (!GetOpenFileName(&of)) return;

	Invocation1 i;
	i.setTarget(this, &Converter::performLoading, std::string(of.lpstrFile));
	i.invokeInMainRunLoop();

	_scene.clear();
	_labStatus->setText("Loading...");
}

void Converter::onBtnSaveClick(et::gui::Button*)
{
	char filename[MAX_PATH] = { };

	OPENFILENAME of = { };
	of.lStructSize = sizeof(of);
	of.hwndOwner = reinterpret_cast<HWND>(application().renderingContextHandle());
	of.hInstance = GetModuleHandle(0);
	of.lpstrFilter = "ET models\0*.etm\0\0";
	of.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	of.lpstrFile = filename;
	of.nMaxFile = MAX_PATH;

	if (!GetSaveFileName(&of)) return;

	Invocation1 i;
	i.setTarget(this, &Converter::performSaving, std::string(of.lpstrFile));
	i.invokeInMainRunLoop();

	_labStatus->setText("Saving...");
}

void Converter::performLoading(std::string path)
{
	lowercase(path);
	size_t value = path.find_last_of(".etm");
	size_t len = path.length();

	if (value == len-1)
	{
		_scene.deserialize(path, _rc, _texCache, 0);
	}
	else
	{
		FBXLoader loader(path);

		s3d::ElementContainer::Pointer loadedScene = loader.load(_rc, _texCache);
		if (loadedScene.valid())
			loadedScene->setParent(&_scene);
	}
	_labStatus->setText("Completed.");
}

void Converter::performSaving(std::string path)
{
	if (path.find_last_of(".etm") != path.length() - 1)
		path += ".etm";

	_scene.serialize(path);
	_labStatus->setText("Completed.");
}