#include "stdafx.h"
#include "Editor.h"
#include "wiInitializer.h"
#include "wiRenderer.h"

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

	wiRenderer::GetDevice()->SetVSyncEnabled(false);
	wiRenderer::EMITTERSENABLED = true;
	wiRenderer::HAIRPARTICLEENABLED = true;
	wiRenderer::LoadDefaultLighting();
	wiRenderer::SetDirectionalLightShadowProps(1024, 2);
	wiRenderer::SetPointLightShadowProps(3, 512);
	wiRenderer::SetSpotLightShadowProps(3, 512);
	wiRenderer::physicsEngine = new wiBULLET();


	wiFont::addFontStyle("basic");
	wiInputManager::GetInstance()->addXInput(new wiXInput());

	EditorComponent* editorComponent = new EditorComponent;
	editorComponent->Initialize();

	activateComponent(editorComponent);

	this->infoDisplay.fpsinfo = true;

	wiRenderer::getCamera()->Translate(XMFLOAT3(0, 1, -10));
}



Model* model = nullptr;
void EditorComponent::Initialize()
{
	setShadowsEnabled(false);

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	wiWindow* materialWindow = new wiWindow(&GetGUI(),"MaterialWindow");
	materialWindow->SetSize(XMFLOAT2(600, 400));
	materialWindow->SetEnabled(false);
	GetGUI().AddWidget(materialWindow);

	wiCheckBox* unshadedCheckBox = new wiCheckBox("Unshaded: ");
	unshadedCheckBox->SetPos(XMFLOAT2(400, 30));
	unshadedCheckBox->OnClick([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->shadeless = args.bValue;
	});
	materialWindow->AddWidget(unshadedCheckBox);

	wiCheckBox* waterCheckBox = new wiCheckBox("Water: ");
	waterCheckBox->SetPos(XMFLOAT2(550, 30));
	waterCheckBox->OnClick([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->water = args.bValue;
	});
	materialWindow->AddWidget(waterCheckBox);

	wiSlider* specPowSlider = new wiSlider(1, 128, 64, 1000, "Specular Power: ");
	specPowSlider->SetSize(XMFLOAT2(100, 20));
	specPowSlider->SetPos(XMFLOAT2(400, 60));
	specPowSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->specular_power = (int)args.fValue;
	});
	materialWindow->AddWidget(specPowSlider);

	wiSlider* metalnessSlider = new wiSlider(0, 1, 0.0, 1000, "Metalness: ");
	metalnessSlider->SetSize(XMFLOAT2(100, 20));
	metalnessSlider->SetPos(XMFLOAT2(400, 90));
	metalnessSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->enviroReflection = args.fValue;
	});
	materialWindow->AddWidget(metalnessSlider);

	wiSlider* roughnessSlider = new wiSlider(0, 1, 0.5, 1000, "Roughness: ");
	roughnessSlider->SetSize(XMFLOAT2(100, 20));
	roughnessSlider->SetPos(XMFLOAT2(400, 120));
	roughnessSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->roughness = args.fValue;
	});
	materialWindow->AddWidget(roughnessSlider);

	wiSlider* alphaSlider = new wiSlider(0, 1, 1.0, 1000, "Alpha: ");
	alphaSlider->SetSize(XMFLOAT2(100, 20));
	alphaSlider->SetPos(XMFLOAT2(400, 150));
	alphaSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->alpha = args.fValue;
	});
	materialWindow->AddWidget(alphaSlider);

	wiSlider* refractionIndexSlider = new wiSlider(0, 1.0, 0.02, 1000, "Refraction Index: ");
	refractionIndexSlider->SetSize(XMFLOAT2(100, 20));
	refractionIndexSlider->SetPos(XMFLOAT2(400, 180));
	refractionIndexSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->refraction_index = args.fValue;
	});
	materialWindow->AddWidget(refractionIndexSlider);

	wiSlider* emissiveSlider = new wiSlider(0, 1000, 0.0, 100000, "Emissive: ");
	emissiveSlider->SetSize(XMFLOAT2(100, 20));
	emissiveSlider->SetPos(XMFLOAT2(400, 210));
	emissiveSlider->OnSlide([=](wiEventArgs args) {
		model->objects[0]->mesh->materials[0]->emissive = args.fValue;
	});
	materialWindow->AddWidget(emissiveSlider);




	wiButton* materialWnd_Toggle = new wiButton("MaterialWnd");
	materialWnd_Toggle->SetPos(XMFLOAT2(0, screenH - 40));
	materialWnd_Toggle->SetSize(XMFLOAT2(100, 40));
	materialWnd_Toggle->SetFontScaling(0.20f);
	materialWnd_Toggle->OnClick([=](wiEventArgs args) {
		materialWindow->SetVisible(!materialWindow->IsVisible());
	});
	GetGUI().AddWidget(materialWnd_Toggle);

	materialWindow->Translate(XMFLOAT3(30, 30, 0));



	wiButton* modelButton = new wiButton("ModelButton");
	modelButton->SetText("Load Model");
	modelButton->SetPos(XMFLOAT2(110, screenH - 40));
	modelButton->SetSize(XMFLOAT2(100, 40));
	modelButton->SetFontScaling(0.25f);
	modelButton->OnClick([=](wiEventArgs args) {
		materialWindow->SetEnabled(false);
		model = wiRenderer::LoadModel("CONTENT/models/barrel/", "barrel");
		if (model == nullptr)
		{
			return;
		}
		for (auto& x : model->objects)
		{
			x->rigidBody = false;
		}
		unshadedCheckBox->SetCheck(model->objects[0]->mesh->materials[0]->shadeless);
		waterCheckBox->SetCheck(model->objects[0]->mesh->materials[0]->water);
		specPowSlider->SetValue((float)model->objects[0]->mesh->materials[0]->specular_power);
		metalnessSlider->SetValue(model->objects[0]->mesh->materials[0]->enviroReflection);
		roughnessSlider->SetValue(model->objects[0]->mesh->materials[0]->roughness);
		alphaSlider->SetValue(model->objects[0]->mesh->materials[0]->alpha);
		refractionIndexSlider->SetValue(model->objects[0]->mesh->materials[0]->refraction_index);
		emissiveSlider->SetValue(model->objects[0]->mesh->materials[0]->emissive);
		materialWindow->SetEnabled(true);
	});
	GetGUI().AddWidget(modelButton);


	wiButton* skyButton = new wiButton("Sky");
	skyButton->SetText("Sky");
	skyButton->SetPos(XMFLOAT2(220, screenH - 40));
	skyButton->SetSize(XMFLOAT2(100, 40));
	skyButton->OnClick([=](wiEventArgs args) {
		static bool toggle = true;
		if (toggle)
		{
			wiRenderer::SetEnviromentMap((Texture2D*)Content.add("CONTENT/scene/env.dds"));
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
	float speed = (wiInputManager::GetInstance()->down(VK_SHIFT) ? 1.0f : 0.1f);
	if (wiInputManager::GetInstance()->down('A')) wiRenderer::getCamera()->Move(XMVectorSet(-speed, 0, 0, 0));
	if (wiInputManager::GetInstance()->down('D')) wiRenderer::getCamera()->Move(XMVectorSet(speed, 0, 0, 0));
	if (wiInputManager::GetInstance()->down('W')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, speed, 0));
	if (wiInputManager::GetInstance()->down('S')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, -speed, 0));
	if (wiInputManager::GetInstance()->down(VK_SPACE)) wiRenderer::getCamera()->Move(XMVectorSet(0, speed, 0, 0));
	if (wiInputManager::GetInstance()->down(VK_CONTROL)) wiRenderer::getCamera()->Move(XMVectorSet(0, -speed, 0, 0));

	static XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 currentMouse = wiInputManager::GetInstance()->getpointer();
	if (wiInputManager::GetInstance()->down(VK_RBUTTON))
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

	__super::Update();
}
void EditorComponent::Unload()
{
	// ...

	__super::Unload();
}
