#include "stdafx.h"
#include "Editor.h"
#include "wiInitializer.h"
#include "wiRenderer.h"
#include "MaterialWindow.h"
#include "PostprocessWindow.h"
#include "WorldWindow.h"
#include "ObjectWindow.h"
#include "MeshWindow.h"
#include "CameraWindow.h"

#include <Commdlg.h> // openfile
#include <WinBase.h>

using namespace wiGraphicsTypes;


Editor::Editor()
{
	SAFE_INIT(renderComponent);
	SAFE_INIT(loader);
}


Editor::~Editor()
{
	//SAFE_DELETE(renderComponent);
	SAFE_DELETE(loader);
}

void Editor::Initialize()
{

	MainComponent::Initialize();

	infoDisplay.active = true;
	infoDisplay.watermark = true;
	infoDisplay.fpsinfo = false;
	infoDisplay.cpuinfo = false;

	wiInitializer::InitializeComponents(
		wiInitializer::WICKEDENGINE_INITIALIZE_RENDERER
		| wiInitializer::WICKEDENGINE_INITIALIZE_IMAGE
		| wiInitializer::WICKEDENGINE_INITIALIZE_FONT
		| wiInitializer::WICKEDENGINE_INITIALIZE_SOUND
		| wiInitializer::WICKEDENGINE_INITIALIZE_MISC
		);

	wiRenderer::GetDevice()->SetVSyncEnabled(true);
	wiRenderer::EMITTERSENABLED = true;
	wiRenderer::HAIRPARTICLEENABLED = true;
	//wiRenderer::LoadDefaultLighting();
	wiRenderer::SetDirectionalLightShadowProps(1024, 2);
	wiRenderer::SetPointLightShadowProps(3, 512);
	wiRenderer::SetSpotLightShadowProps(3, 512);
	wiRenderer::physicsEngine = new wiBULLET();


	wiFont::addFontStyle("basic");
	wiInputManager::GetInstance()->addXInput(new wiXInput());

	renderComponent = new EditorComponent;
	renderComponent->Initialize();
	loader = new EditorLoadingScreen;
	loader->Initialize();

	renderComponent->loader = loader;
	renderComponent->main = this;

	loader->addLoadingComponent(renderComponent, this);

	activateComponent(loader);

	this->infoDisplay.fpsinfo = true;

	wiRenderer::getCamera()->Translate(XMFLOAT3(0, 1, -10));

}

void SplitFilename(const string& path, string& folder, string& file)
{
	size_t found;
	found = path.find_last_of("/\\");
	folder = path.substr(0, found + 1);
	file = path.substr(found + 1);
}


void EditorLoadingScreen::Compose()
{
	wiFont("Loading...", wiFontProps(wiRenderer::GetDevice()->GetScreenWidth() * 0.5f, wiRenderer::GetDevice()->GetScreenHeight() * 0.5f, 22,
		WIFALIGN_MID, WIFALIGN_MID)).Draw();
}

wiTranslator* translator = nullptr;
bool translator_active = false;
Transform* translatedEntity = nullptr;
Transform* translatedEntity_Parent = nullptr;
void BeginTranslate(Transform* entity)
{
	translatedEntity = entity;
	translator_active = true;
	translator->Clear();
	translator->Translate(translatedEntity->translation);
	translatedEntity_Parent = translatedEntity->parent;
	translatedEntity->detach();
	translatedEntity->attachTo(translator);
}
void EndTranslate()
{
	translator_active = false;
	translator->detach();
	if (translatedEntity != nullptr)
	{
		translatedEntity->attachTo(translatedEntity_Parent);
	}
}

void EditorComponent::Initialize()
{
	setShadowsEnabled(true);
	setReflectionsEnabled(true);
	setSSAOEnabled(false);
	setSSREnabled(false);
	setMotionBlurEnabled(false);
	setColorGradingEnabled(false);
	setEyeAdaptionEnabled(false);
	setFXAAEnabled(false);
	setDepthOfFieldEnabled(false);
	setLightShaftsEnabled(false);

	SAFE_INIT(materialWnd);
	SAFE_INIT(postprocessWnd);
	SAFE_INIT(worldWnd);
	SAFE_INIT(objectWnd);
	SAFE_INIT(meshWnd);
	SAFE_INIT(cameraWnd);

	__super::Initialize();
}
void EditorComponent::Load()
{
	__super::Load();


	translator = new wiTranslator;

	materialWnd = new MaterialWindow(&GetGUI());
	postprocessWnd = new PostprocessWindow(this);
	worldWnd = new WorldWindow(&GetGUI());
	objectWnd = new ObjectWindow(&GetGUI());
	meshWnd = new MeshWindow(&GetGUI());
	cameraWnd = new CameraWindow(&GetGUI());

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	wiButton* worldWnd_Toggle = new wiButton("World");
	worldWnd_Toggle->SetPos(XMFLOAT2(0, screenH - 40));
	worldWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	worldWnd_Toggle->SetFontScaling(0.4f);
	worldWnd_Toggle->OnClick([=](wiEventArgs args) {
		worldWnd->worldWindow->SetVisible(!worldWnd->worldWindow->IsVisible());
	});
	GetGUI().AddWidget(worldWnd_Toggle);

	wiButton* objectWnd_Toggle = new wiButton("Object");
	objectWnd_Toggle->SetPos(XMFLOAT2(105, screenH - 40));
	objectWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	objectWnd_Toggle->SetFontScaling(0.4f);
	objectWnd_Toggle->OnClick([=](wiEventArgs args) {
		objectWnd->objectWindow->SetVisible(!objectWnd->objectWindow->IsVisible());
	});
	GetGUI().AddWidget(objectWnd_Toggle);

	wiButton* meshWnd_Toggle = new wiButton("Mesh");
	meshWnd_Toggle->SetPos(XMFLOAT2(210, screenH - 40));
	meshWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	meshWnd_Toggle->SetFontScaling(0.4f);
	meshWnd_Toggle->OnClick([=](wiEventArgs args) {
		meshWnd->meshWindow->SetVisible(!meshWnd->meshWindow->IsVisible());
	});
	GetGUI().AddWidget(meshWnd_Toggle);

	wiButton* materialWnd_Toggle = new wiButton("Material");
	materialWnd_Toggle->SetPos(XMFLOAT2(315, screenH - 40));
	materialWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	materialWnd_Toggle->SetFontScaling(0.3f);
	materialWnd_Toggle->OnClick([=](wiEventArgs args) {
		materialWnd->materialWindow->SetVisible(!materialWnd->materialWindow->IsVisible());
	});
	GetGUI().AddWidget(materialWnd_Toggle);

	wiButton* postprocessWnd_Toggle = new wiButton("PostProcess");
	postprocessWnd_Toggle->SetPos(XMFLOAT2(420, screenH - 40));
	postprocessWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	postprocessWnd_Toggle->SetFontScaling(0.22f);
	postprocessWnd_Toggle->OnClick([=](wiEventArgs args) {
		postprocessWnd->ppWindow->SetVisible(!postprocessWnd->ppWindow->IsVisible());
	});
	GetGUI().AddWidget(postprocessWnd_Toggle);

	wiButton* cameraWnd_Toggle = new wiButton("Camera");
	cameraWnd_Toggle->SetPos(XMFLOAT2(525, screenH - 40));
	cameraWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	cameraWnd_Toggle->SetFontScaling(0.3f);
	cameraWnd_Toggle->OnClick([=](wiEventArgs args) {
		cameraWnd->cameraWindow->SetVisible(!cameraWnd->cameraWindow->IsVisible());
	});
	GetGUI().AddWidget(cameraWnd_Toggle);


	////////////////////////////////////////////////////////////////////////////////////



	wiCheckBox* translatorCheckBox = new wiCheckBox("Translator: ");
	translatorCheckBox->SetPos(XMFLOAT2(screenW - 335, 0));
	translatorCheckBox->OnClick([=](wiEventArgs args) {
		translator->enabled = args.bValue;
	});
	GetGUI().AddWidget(translatorCheckBox);



	wiButton* modelButton = new wiButton("LoadModel");
	modelButton->SetPos(XMFLOAT2(screenW - 310, 0));
	modelButton->SetSize(XMFLOAT2(100, 40));
	modelButton->SetFontScaling(0.25f);
	modelButton->OnClick([=](wiEventArgs args) {
		//wiRenderer::CleanUpStaticTemp();
		//wiRenderer::LoadModel("CONTENT/models/cube/", "cube");

		//wiRenderer::LoadModel("CONTENT/models/Havoc/1/", "Havoc");
		//wiRenderer::LoadModel("CONTENT/models/instanceBenchmark2/", "instanceBenchmark2");

		thread([&] {
			char szFile[260];

			OPENFILENAMEA ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Wicked Model Format\0*.wio\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileNameA(&ofn) == TRUE) {
				string fileName = ofn.lpstrFile;
				string dir, file;
				SplitFilename(fileName, dir, file);
				file = file.substr(0, file.length() - 4);

				loader->addLoadingFunction([=] {
					wiRenderer::LoadModel(dir, file);
				});
				loader->onFinished([=] {
					main->activateComponent(this);
					worldWnd->UpdateFromRenderer();
				});
				main->activateComponent(loader);
			}
		}).detach();
	});
	GetGUI().AddWidget(modelButton);


	wiButton* skyButton = new wiButton("LoadSky");
	skyButton->SetPos(XMFLOAT2(screenW - 205, 0));
	skyButton->SetSize(XMFLOAT2(100, 18));
	skyButton->SetFontScaling(0.5f);
	skyButton->OnClick([=](wiEventArgs args) {
		//wiRenderer::SetEnviromentMap((Texture2D*)Content.add("CONTENT/env.dds"));

		auto x = wiRenderer::GetEnviromentMap();

		if (x == nullptr)
		{
			thread([&] {
				char szFile[260];

				OPENFILENAMEA ofn;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = szFile;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Cubemap texture\0*.dds\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetOpenFileNameA(&ofn) == TRUE) {
					string fileName = ofn.lpstrFile;
					wiRenderer::SetEnviromentMap((Texture2D*)Content.add(fileName));
				}
			}).detach();
		}
		else
		{
			wiRenderer::SetEnviromentMap(nullptr);
		}

	});
	GetGUI().AddWidget(skyButton);


	wiButton* colorGradingButton = new wiButton("ColorGrading");
	colorGradingButton->SetPos(XMFLOAT2(screenW - 205, 22));
	colorGradingButton->SetSize(XMFLOAT2(100, 18));
	colorGradingButton->SetFontScaling(0.45f);
	colorGradingButton->OnClick([=](wiEventArgs args) {
		auto x = wiRenderer::GetColorGrading();

		if (x == nullptr)
		{
			thread([&] {
				char szFile[260];

				OPENFILENAMEA ofn;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = szFile;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Color Grading texture\0*.dds\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetOpenFileNameA(&ofn) == TRUE) {
					string fileName = ofn.lpstrFile;
					wiRenderer::SetColorGrading((Texture2D*)Content.add(fileName));
				}
			}).detach();
		}
		else
		{
			wiRenderer::SetColorGrading(nullptr);
		}

	});
	GetGUI().AddWidget(colorGradingButton);


	wiButton* clearButton = new wiButton("ClearWorld");
	clearButton->SetPos(XMFLOAT2(screenW - 100, 0));
	clearButton->SetSize(XMFLOAT2(100, 40));
	clearButton->SetFontScaling(0.25f);
	clearButton->OnClick([](wiEventArgs args) {
		wiRenderer::CleanUpStaticTemp();
		EndTranslate();
	});
	GetGUI().AddWidget(clearButton);

	// ...
}
void EditorComponent::Start()
{
	__super::Start();
}
void EditorComponent::Update()
{
	if (!wiBackLog::isActive())
	{

		float speed = (wiInputManager::GetInstance()->down(VK_SHIFT) ? 1.0f : 0.1f);
		if (wiInputManager::GetInstance()->down('A')) wiRenderer::getCamera()->Move(XMVectorSet(-speed, 0, 0, 0));
		if (wiInputManager::GetInstance()->down('D')) wiRenderer::getCamera()->Move(XMVectorSet(speed, 0, 0, 0));
		if (wiInputManager::GetInstance()->down('W')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, speed, 0));
		if (wiInputManager::GetInstance()->down('S')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, -speed, 0));
		if (wiInputManager::GetInstance()->down(VK_SPACE)) wiRenderer::getCamera()->Move(XMVectorSet(0, speed, 0, 0));
		if (wiInputManager::GetInstance()->down(VK_CONTROL)) wiRenderer::getCamera()->Move(XMVectorSet(0, -speed, 0, 0));

		static XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
		XMFLOAT4 currentMouse = wiInputManager::GetInstance()->getpointer();
		if (wiInputManager::GetInstance()->down(VK_MBUTTON))
		{
			float xDif = currentMouse.x - originalMouse.x;
			float yDif = currentMouse.y - originalMouse.y;
			wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(0, 0.1f*xDif*(1.0f / 60.0f), 0));
			wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(0.1f*yDif*(1.0f / 60.0f), 0, 0));
			wiInputManager::GetInstance()->setpointer(originalMouse);
		}
		else
		{
			originalMouse = wiInputManager::GetInstance()->getpointer();
		}

		if (wiInputManager::GetInstance()->press(VK_RBUTTON))
		{
			wiRenderer::Picked pick = wiRenderer::Pick((long)currentMouse.x, (long)currentMouse.y, 
				PICK_OPAQUE | PICK_TRANSPARENT | PICK_WATER);

			objectWnd->SetObject(pick.object);

			if (pick.object != nullptr)
			{
				meshWnd->SetMesh(pick.object->mesh);
				if (pick.subsetIndex < (int)pick.object->mesh->subsets.size())
				{
					Material* material = pick.object->mesh->subsets[pick.subsetIndex].material;

					materialWnd->SetMaterial(material);
				}

				EndTranslate();
				BeginTranslate(pick.object);
			}
			else
			{
				meshWnd->SetMesh(nullptr);
				materialWnd->SetMaterial(nullptr);

				EndTranslate();
			}
		}

	}

	translator->Update();

	__super::Update();

	if (translator_active)
	{
		wiRenderer::AddRenderableTranslator(translator);
	}
}
void EditorComponent::Unload()
{
	// ...
	SAFE_DELETE(materialWnd);
	SAFE_DELETE(postprocessWnd);
	SAFE_DELETE(worldWnd);
	SAFE_DELETE(objectWnd);
	SAFE_DELETE(meshWnd);
	SAFE_DELETE(cameraWnd);

	SAFE_DELETE(translator);

	__super::Unload();
}
