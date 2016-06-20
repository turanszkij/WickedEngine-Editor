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
#include "RendererWindow.h"
#include "EnvProbeWindow.h"
#include "DecalWindow.h"
#include "LightWindow.h"

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
	//SAFE_DELETE(loader);
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
	loader->Load();

	renderComponent->loader = loader;
	renderComponent->main = this;

	loader->addLoadingComponent(renderComponent, this);

	activateComponent(loader);

	this->infoDisplay.fpsinfo = true;

}

void SplitFilename(const string& path, string& folder, string& file)
{
	size_t found;
	found = path.find_last_of("/\\");
	folder = path.substr(0, found + 1);
	file = path.substr(found + 1);
}

void EditorLoadingScreen::Load()
{
	sprite = wiSprite();
	sprite.setTexture(wiTextureHelper::getInstance()->getWhite());
	sprite.anim.rot = 0.05f;
	sprite.effects.pos = XMFLOAT3(wiRenderer::GetDevice()->GetScreenWidth()*0.9f, wiRenderer::GetDevice()->GetScreenHeight()*0.8f, 0);
	sprite.effects.siz = XMFLOAT2(50, 50);
	sprite.effects.pivot = XMFLOAT2(0.5f, 0.5f);
	addSprite(&sprite);

	__super::Load();
}
void EditorLoadingScreen::Compose()
{
	__super::Compose();

	wiFont("Loading...", wiFontProps(wiRenderer::GetDevice()->GetScreenWidth() * 0.5f, wiRenderer::GetDevice()->GetScreenHeight() * 0.5f, 22,
		WIFALIGN_MID, WIFALIGN_MID)).Draw();
}
void EditorLoadingScreen::Unload()
{

}

wiTranslator* translator = nullptr;
bool translator_active = false;
list<wiRenderer::Picked> selected;
map<Transform*,Transform*> savedParents;
void BeginTranslate()
{
	translator_active = true;
	translator->Clear();

	XMVECTOR centerV = XMVectorSet(0, 0, 0, 0);
	float count = 0;
	for (auto& x : selected)
	{
		if (x.transform != nullptr)
		{
			centerV = XMVectorAdd(centerV, XMLoadFloat3(&x.transform->translation));
			count += 1.0f;
		}
	}
	if (count > 0)
	{
		centerV /= count;
		XMFLOAT3 center;
		XMStoreFloat3(&center, centerV);
		translator->Translate(center);
		for (auto& x : selected)
		{
			if (x.transform != nullptr)
			{
				x.transform->detach();
				x.transform->attachTo(translator);
			}
		}
	}
}
void EndTranslate()
{
	translator_active = false;
	translator->detach();

	for (auto& x : selected)
	{
		if (x.transform != nullptr)
		{
			x.transform->detach();
			map<Transform*,Transform*>::iterator it = savedParents.find(x.transform);
			if (it != savedParents.end())
			{
				x.transform->attachTo(it->second);
			}
		}
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
	SAFE_INIT(rendererWnd);

	__super::Initialize();
}
void EditorComponent::Load()
{
	__super::Load();


	translator = new wiTranslator;
	translator->enabled = false;

	materialWnd = new MaterialWindow(&GetGUI());
	postprocessWnd = new PostprocessWindow(this);
	worldWnd = new WorldWindow(&GetGUI());
	objectWnd = new ObjectWindow(&GetGUI());
	meshWnd = new MeshWindow(&GetGUI());
	cameraWnd = new CameraWindow(&GetGUI());
	rendererWnd = new RendererWindow(&GetGUI());
	envProbeWnd = new EnvProbeWindow(&GetGUI());
	decalWnd = new DecalWindow(&GetGUI());
	lightWnd = new LightWindow(&GetGUI());

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	float step = 105, x = -step;

	wiButton* rendererWnd_Toggle = new wiButton("Renderer");
	rendererWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	rendererWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	rendererWnd_Toggle->SetFontScaling(0.3f);
	rendererWnd_Toggle->OnClick([=](wiEventArgs args) {
		rendererWnd->rendererWindow->SetVisible(!rendererWnd->rendererWindow->IsVisible());
	});
	GetGUI().AddWidget(rendererWnd_Toggle);

	wiButton* worldWnd_Toggle = new wiButton("World");
	worldWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	worldWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	worldWnd_Toggle->SetFontScaling(0.4f);
	worldWnd_Toggle->OnClick([=](wiEventArgs args) {
		worldWnd->worldWindow->SetVisible(!worldWnd->worldWindow->IsVisible());
	});
	GetGUI().AddWidget(worldWnd_Toggle);

	wiButton* objectWnd_Toggle = new wiButton("Object");
	objectWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	objectWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	objectWnd_Toggle->SetFontScaling(0.4f);
	objectWnd_Toggle->OnClick([=](wiEventArgs args) {
		objectWnd->objectWindow->SetVisible(!objectWnd->objectWindow->IsVisible());
	});
	GetGUI().AddWidget(objectWnd_Toggle);

	wiButton* meshWnd_Toggle = new wiButton("Mesh");
	meshWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	meshWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	meshWnd_Toggle->SetFontScaling(0.4f);
	meshWnd_Toggle->OnClick([=](wiEventArgs args) {
		meshWnd->meshWindow->SetVisible(!meshWnd->meshWindow->IsVisible());
	});
	GetGUI().AddWidget(meshWnd_Toggle);

	wiButton* materialWnd_Toggle = new wiButton("Material");
	materialWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	materialWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	materialWnd_Toggle->SetFontScaling(0.3f);
	materialWnd_Toggle->OnClick([=](wiEventArgs args) {
		materialWnd->materialWindow->SetVisible(!materialWnd->materialWindow->IsVisible());
	});
	GetGUI().AddWidget(materialWnd_Toggle);

	wiButton* postprocessWnd_Toggle = new wiButton("PostProcess");
	postprocessWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	postprocessWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	postprocessWnd_Toggle->SetFontScaling(0.22f);
	postprocessWnd_Toggle->OnClick([=](wiEventArgs args) {
		postprocessWnd->ppWindow->SetVisible(!postprocessWnd->ppWindow->IsVisible());
	});
	GetGUI().AddWidget(postprocessWnd_Toggle);

	wiButton* cameraWnd_Toggle = new wiButton("Camera");
	cameraWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	cameraWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	cameraWnd_Toggle->SetFontScaling(0.3f);
	cameraWnd_Toggle->OnClick([=](wiEventArgs args) {
		cameraWnd->cameraWindow->SetVisible(!cameraWnd->cameraWindow->IsVisible());
	});
	GetGUI().AddWidget(cameraWnd_Toggle);

	wiButton* envProbeWnd_Toggle = new wiButton("EnvProbe");
	envProbeWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	envProbeWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	envProbeWnd_Toggle->SetFontScaling(0.3f);
	envProbeWnd_Toggle->OnClick([=](wiEventArgs args) {
		envProbeWnd->envProbeWindow->SetVisible(!envProbeWnd->envProbeWindow->IsVisible());
	});
	GetGUI().AddWidget(envProbeWnd_Toggle);

	wiButton* decalWnd_Toggle = new wiButton("Decal");
	decalWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	decalWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	decalWnd_Toggle->SetFontScaling(0.3f);
	decalWnd_Toggle->OnClick([=](wiEventArgs args) {
		decalWnd->decalWindow->SetVisible(!decalWnd->decalWindow->IsVisible());
	});
	GetGUI().AddWidget(decalWnd_Toggle);

	wiButton* lightWnd_Toggle = new wiButton("Light");
	lightWnd_Toggle->SetPos(XMFLOAT2(x += step, screenH - 40));
	lightWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	lightWnd_Toggle->SetFontScaling(0.3f);
	lightWnd_Toggle->OnClick([=](wiEventArgs args) {
		lightWnd->lightWindow->SetVisible(!lightWnd->lightWindow->IsVisible());
	});
	GetGUI().AddWidget(lightWnd_Toggle);


	////////////////////////////////////////////////////////////////////////////////////



	wiCheckBox* translatorCheckBox = new wiCheckBox("Translator: ");
	translatorCheckBox->SetPos(XMFLOAT2(screenW - 495, 0));
	translatorCheckBox->SetSize(XMFLOAT2(18, 18));
	translatorCheckBox->OnClick([=](wiEventArgs args) {
		if(!args.bValue)
			EndTranslate();
		translator->enabled = args.bValue;
	});
	GetGUI().AddWidget(translatorCheckBox);

	wiCheckBox* isScalatorCheckBox = new wiCheckBox("S:");
	isScalatorCheckBox->SetPos(XMFLOAT2(screenW - 575, 22));
	isScalatorCheckBox->SetSize(XMFLOAT2(18, 18));
	isScalatorCheckBox->OnClick([=](wiEventArgs args) {
		translator->isScalator = args.bValue;
	});
	isScalatorCheckBox->SetCheck(translator->isScalator);
	GetGUI().AddWidget(isScalatorCheckBox);

	wiCheckBox* isRotatorCheckBox = new wiCheckBox("R:");
	isRotatorCheckBox->SetPos(XMFLOAT2(screenW - 535, 22));
	isRotatorCheckBox->SetSize(XMFLOAT2(18, 18));
	isRotatorCheckBox->OnClick([=](wiEventArgs args) {
		translator->isRotator = args.bValue;
	});
	isRotatorCheckBox->SetCheck(translator->isRotator);
	GetGUI().AddWidget(isRotatorCheckBox);

	wiCheckBox* isTranslatorCheckBox = new wiCheckBox("T:");
	isTranslatorCheckBox->SetPos(XMFLOAT2(screenW - 495, 22));
	isTranslatorCheckBox->SetSize(XMFLOAT2(18, 18));
	isTranslatorCheckBox->OnClick([=](wiEventArgs args) {
		translator->isTranslator = args.bValue;
	});
	isTranslatorCheckBox->SetCheck(translator->isTranslator);
	GetGUI().AddWidget(isTranslatorCheckBox);


	wiButton* saveButton = new wiButton("Save");
	saveButton->SetPos(XMFLOAT2(screenW - 470, 0));
	saveButton->SetSize(XMFLOAT2(100, 40));
	saveButton->SetFontScaling(0.25f);
	saveButton->SetColor(wiColor(0, 198, 101, 200), wiWidget::WIDGETSTATE::IDLE);
	saveButton->SetColor(wiColor(0, 255, 140, 255), wiWidget::WIDGETSTATE::FOCUS);
	saveButton->OnClick([=](wiEventArgs args) {
		EndTranslate();

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
		ofn.lpstrFilter = "Wicked Model Format\0*.wimf\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_OVERWRITEPROMPT;
		if (GetSaveFileNameA(&ofn) == TRUE) {
			string fileName = ofn.lpstrFile;
			if (fileName.substr(fileName.length() - 5).compare(".wimf") != 0)
			{
				fileName += ".wimf";
			}
			wiArchive archive(fileName, false);
			if (archive.IsOpen())
			{
				auto& children = wiRenderer::GetScene().GetWorldNode()->children;
				for(auto& x : children)
				{
					Model* model = dynamic_cast<Model*>(x);
					if (model != nullptr)
					{
						model->Serialize(archive);
						break;
					}
				}
			}
			else
			{
				wiHelper::messageBox("Could not create " + fileName + "!");
			}
		}
	});
	GetGUI().AddWidget(saveButton);


	wiButton* modelButton = new wiButton("LoadModel");
	modelButton->SetPos(XMFLOAT2(screenW - 365, 0));
	modelButton->SetSize(XMFLOAT2(100, 40));
	modelButton->SetFontScaling(0.25f);
	modelButton->SetColor(wiColor(0, 89, 255, 200), wiWidget::WIDGETSTATE::IDLE);
	modelButton->SetColor(wiColor(112, 155, 255, 255), wiWidget::WIDGETSTATE::FOCUS);
	modelButton->OnClick([=](wiEventArgs args) {
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
			ofn.lpstrFilter = "Wicked Model Format\0*.wimf;*.wio\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileNameA(&ofn) == TRUE) {
				string fileName = ofn.lpstrFile;
				string dir, file;
				SplitFilename(fileName, dir, file);

				if (fileName.substr(fileName.length() - 5).compare(".wimf") == 0)
				{
					file = file.substr(0, file.length() - 5);
				}
				else
				{
					file = file.substr(0, file.length() - 4);
				}

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
	skyButton->SetPos(XMFLOAT2(screenW - 260, 0));
	skyButton->SetSize(XMFLOAT2(100, 18));
	skyButton->SetFontScaling(0.5f);
	skyButton->SetColor(wiColor(0, 89, 255, 200), wiWidget::WIDGETSTATE::IDLE);
	skyButton->SetColor(wiColor(112, 155, 255, 255), wiWidget::WIDGETSTATE::FOCUS);
	skyButton->OnClick([=](wiEventArgs args) {
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
	colorGradingButton->SetPos(XMFLOAT2(screenW - 260, 22));
	colorGradingButton->SetSize(XMFLOAT2(100, 18));
	colorGradingButton->SetFontScaling(0.45f);
	colorGradingButton->SetColor(wiColor(0, 89, 255, 200), wiWidget::WIDGETSTATE::IDLE);
	colorGradingButton->SetColor(wiColor(112, 155, 255, 255), wiWidget::WIDGETSTATE::FOCUS);
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
	clearButton->SetPos(XMFLOAT2(screenW - 155, 0));
	clearButton->SetSize(XMFLOAT2(100, 40));
	clearButton->SetFontScaling(0.25f);
	clearButton->SetColor(wiColor(190, 0, 0, 200), wiWidget::WIDGETSTATE::IDLE);
	clearButton->SetColor(wiColor(255, 0, 0, 255), wiWidget::WIDGETSTATE::FOCUS);
	clearButton->OnClick([](wiEventArgs args) {
		selected.clear();
		EndTranslate();
		wiRenderer::CleanUpStaticTemp();
	});
	GetGUI().AddWidget(clearButton);


	wiButton* exitButton = new wiButton("X");
	exitButton->SetPos(XMFLOAT2(screenW - 50, 0));
	exitButton->SetSize(XMFLOAT2(50, 40));
	exitButton->SetFontScaling(0.25f);
	exitButton->SetColor(wiColor(190, 0, 0, 200), wiWidget::WIDGETSTATE::IDLE);
	exitButton->SetColor(wiColor(255, 0, 0, 255), wiWidget::WIDGETSTATE::FOCUS);
	exitButton->OnClick([](wiEventArgs args) {
		exit(0);
	});
	GetGUI().AddWidget(exitButton);

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
			wiRenderer::Picked picked = wiRenderer::Pick((long)currentMouse.x, (long)currentMouse.y, rendererWnd->GetPickType());

			if (!selected.empty() && wiInputManager::GetInstance()->down(VK_LSHIFT))
			{
				list<wiRenderer::Picked>::iterator it = selected.begin();
				for (; it != selected.end(); ++it)
				{
					if ((*it) == picked)
					{
						break;
					}
				}
				if (it==selected.end())
				{
					selected.push_back(picked);
					savedParents.insert(pair<Transform*, Transform*>(picked.transform, picked.transform->parent));
				}
				else
				{
					EndTranslate();
					selected.erase(it);
					savedParents.erase(it->transform);
				}
			}
			else
			{
				EndTranslate();
				selected.clear();
				selected.push_back(picked);
				savedParents.clear();
				if (picked.transform != nullptr)
				{
					savedParents.insert(pair<Transform*, Transform*>(picked.transform, picked.transform->parent));
				}
			}

			objectWnd->SetObject(picked.object);

			if (picked.transform != nullptr)
			{
				EndTranslate();

				if (picked.object != nullptr)
				{
					meshWnd->SetMesh(picked.object->mesh);
					if (picked.subsetIndex < (int)picked.object->mesh->subsets.size())
					{
						Material* material = picked.object->mesh->subsets[picked.subsetIndex].material;

						materialWnd->SetMaterial(material);
					}
					if (picked.object->isArmatureDeformed())
					{
						picked.transform = picked.object->mesh->armature;
					}
				}
				else
				{
					meshWnd->SetMesh(nullptr);
					materialWnd->SetMaterial(nullptr);
				}

				if (picked.light != nullptr)
				{
				}
				lightWnd->SetLight(picked.light);
				if (picked.decal != nullptr)
				{
				}
				decalWnd->SetDecal(picked.decal);

				BeginTranslate();

			}
			else
			{
				meshWnd->SetMesh(nullptr);
				materialWnd->SetMaterial(nullptr);

				EndTranslate();
				selected.clear();
			}
		}

	}

	translator->Update();

	__super::Update();
}
void EditorComponent::Render()
{
	if (!selected.empty())
	{
		if (translator_active)
		{
			wiRenderer::AddRenderableTranslator(translator);
		}

		AABB selectedAABB = AABB(XMFLOAT3(FLOAT32_MAX, FLOAT32_MAX, FLOAT32_MAX),XMFLOAT3(-FLOAT32_MAX, -FLOAT32_MAX, -FLOAT32_MAX));
		for (auto& picked : selected)
		{
			if (picked.object != nullptr)
			{
				selectedAABB = AABB::Merge(selectedAABB, picked.object->bounds);
			}
			if (picked.light != nullptr)
			{
				selectedAABB = AABB::Merge(selectedAABB, picked.light->bounds);
			}
			if (picked.decal != nullptr)
			{
				selectedAABB = AABB::Merge(selectedAABB, picked.decal->bounds);

				XMFLOAT4X4 selectionBox;
				selectionBox = picked.decal->world;
				wiRenderer::AddRenderableBox(selectionBox, XMFLOAT4(1, 0, 1, 1));
			}
		}

		XMFLOAT4X4 selectionBox;
		XMStoreFloat4x4(&selectionBox, selectedAABB.getAsBoxMatrix());
		wiRenderer::AddRenderableBox(selectionBox, XMFLOAT4(1, 1, 1, 1));
	}

	__super::Render();
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
	SAFE_DELETE(decalWnd);
	SAFE_DELETE(lightWnd);

	SAFE_DELETE(translator);

	__super::Unload();
}
