#include "stdafx.h"
#include "Editor.h"
#include "wiInitializer.h"
#include "wiRenderer.h"
#include "MaterialWindow.h"
#include "PostprocessWindow.h"
#include "WorldWindow.h"
#include "ObjectWindow.h"
#include "MeshWindow.h"

using namespace wiGraphicsTypes;


Editor::Editor()
{
}


Editor::~Editor()
{
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
	//wiRenderer::physicsEngine = new wiBULLET();


	wiFont::addFontStyle("basic");
	wiInputManager::GetInstance()->addXInput(new wiXInput());

	EditorComponent* editorComponent = new EditorComponent;
	editorComponent->Initialize();

	activateComponent(editorComponent);

	this->infoDisplay.fpsinfo = true;

	wiRenderer::getCamera()->Translate(XMFLOAT3(0, 1, -10));

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

	materialWnd = new MaterialWindow(&GetGUI());
	postprocessWnd = new PostprocessWindow(this);
	worldWnd = new WorldWindow;
	objectWnd = new ObjectWindow(&GetGUI());
	meshWnd = new MeshWindow;

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	wiButton* worldWnd_Toggle = new wiButton("World");
	worldWnd_Toggle->SetPos(XMFLOAT2(0, screenH - 40));
	worldWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	worldWnd_Toggle->SetFontScaling(0.4f);
	worldWnd_Toggle->OnClick([=](wiEventArgs args) {
		wiHelper::messageBox("TODO");
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
		wiHelper::messageBox("TODO");
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



	wiButton* modelButton = new wiButton("LoadModel");
	modelButton->SetPos(XMFLOAT2(screenW-205, 0));
	modelButton->SetSize(XMFLOAT2(100, 40));
	modelButton->SetFontScaling(0.25f);
	modelButton->OnClick([=](wiEventArgs args) {
		wiRenderer::CleanUpStaticTemp();
		wiRenderer::LoadModel("CONTENT/models/instanceBenchmark2/", "instanceBenchmark2");
		//wiRenderer::LoadModel("CONTENT/models/cube/", "cube");
	});
	GetGUI().AddWidget(modelButton);


	wiButton* skyButton = new wiButton("LoadSky");
	skyButton->SetPos(XMFLOAT2(screenW-100, 0));
	skyButton->SetSize(XMFLOAT2(100, 40));
	skyButton->SetFontScaling(0.25f);
	skyButton->OnClick([=](wiEventArgs args) {
		static bool toggle = true;
		if (toggle)
		{
			wiRenderer::SetEnviromentMap((Texture2D*)Content.add("CONTENT/env.dds"));
		}
		else
		{
			wiRenderer::SetEnviromentMap(nullptr);
		}
		toggle = !toggle;
	});
	GetGUI().AddWidget(skyButton);


	__super::Initialize();
}
void EditorComponent::Load()
{
	__super::Load();

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
				if (pick.subsetIndex < (int)pick.object->mesh->subsets.size())
				{
					Material* material = pick.object->mesh->subsets[pick.subsetIndex].material;

					materialWnd->SelectMaterial(material);
				}

			}
			else
			{
				materialWnd->SelectMaterial(nullptr);
			}
		}

	}

	__super::Update();
}
void EditorComponent::Unload()
{
	// ...
	SAFE_DELETE(materialWnd);
	SAFE_DELETE(postprocessWnd);
	SAFE_DELETE(worldWnd);
	SAFE_DELETE(objectWnd);
	SAFE_DELETE(meshWnd);

	__super::Unload();
}
