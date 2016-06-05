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



Material* material = nullptr;

wiWindow* materialWindow = nullptr;
wiLabel* materialLabel = nullptr;
wiCheckBox* waterCheckBox = nullptr; 
wiSlider* normalMapSlider = nullptr;
wiSlider* roughnessSlider = nullptr;
wiSlider* reflectanceSlider = nullptr;
wiSlider* metalnessSlider = nullptr;
wiSlider* alphaSlider = nullptr;
wiSlider* refractionIndexSlider = nullptr;
wiSlider* emissiveSlider = nullptr;
wiSlider* sssSlider = nullptr;

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

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	materialWindow = new wiWindow(&GetGUI(),"MaterialWindow");
	materialWindow->SetSize(XMFLOAT2(600, 400));
	materialWindow->SetEnabled(false);
	GetGUI().AddWidget(materialWindow);

	materialLabel = new wiLabel("Material: ");
	materialLabel->SetPos(XMFLOAT2(10, 30));
	materialLabel->SetSize(XMFLOAT2(300, 20));
	materialLabel->SetText("");
	materialWindow->AddWidget(materialLabel);

	waterCheckBox = new wiCheckBox("Water: ");
	waterCheckBox->SetPos(XMFLOAT2(470, 30));
	waterCheckBox->OnClick([=](wiEventArgs args) {
		material->water = args.bValue;
	});
	materialWindow->AddWidget(waterCheckBox);

	normalMapSlider = new wiSlider(0, 4, 1, 4000, "Normalmap: ");
	normalMapSlider->SetSize(XMFLOAT2(100, 30));
	normalMapSlider->SetPos(XMFLOAT2(400, 60));
	normalMapSlider->OnSlide([=](wiEventArgs args) {
		material->normalMapStrength = args.fValue;
	});
	materialWindow->AddWidget(normalMapSlider);

	roughnessSlider = new wiSlider(0, 1, 0.5, 1000, "Roughness: ");
	roughnessSlider->SetSize(XMFLOAT2(100, 30));
	roughnessSlider->SetPos(XMFLOAT2(400, 100));
	roughnessSlider->OnSlide([=](wiEventArgs args) {
		material->roughness = args.fValue;
	});
	materialWindow->AddWidget(roughnessSlider);

	reflectanceSlider = new wiSlider(0, 1, 0.5, 1000, "Reflectance: ");
	reflectanceSlider->SetSize(XMFLOAT2(100, 30));
	reflectanceSlider->SetPos(XMFLOAT2(400, 140));
	reflectanceSlider->OnSlide([=](wiEventArgs args) {
		material->reflectance = args.fValue;
	});
	materialWindow->AddWidget(reflectanceSlider);

	metalnessSlider = new wiSlider(0, 1, 0.0, 1000, "Metalness: ");
	metalnessSlider->SetSize(XMFLOAT2(100, 30));
	metalnessSlider->SetPos(XMFLOAT2(400, 180));
	metalnessSlider->OnSlide([=](wiEventArgs args) {
		material->metalness = args.fValue;
	});
	materialWindow->AddWidget(metalnessSlider);

	alphaSlider = new wiSlider(0, 1, 1.0, 1000, "Alpha: ");
	alphaSlider->SetSize(XMFLOAT2(100, 30));
	alphaSlider->SetPos(XMFLOAT2(400, 220));
	alphaSlider->OnSlide([=](wiEventArgs args) {
		material->alpha = args.fValue;
	});
	materialWindow->AddWidget(alphaSlider);

	refractionIndexSlider = new wiSlider(0, 1.0, 0.02, 1000, "Refraction Index: ");
	refractionIndexSlider->SetSize(XMFLOAT2(100, 30));
	refractionIndexSlider->SetPos(XMFLOAT2(400, 260));
	refractionIndexSlider->OnSlide([=](wiEventArgs args) {
		material->refractionIndex = args.fValue;
	});
	materialWindow->AddWidget(refractionIndexSlider);

	emissiveSlider = new wiSlider(0, 1, 0.0, 1000, "Emissive: ");
	emissiveSlider->SetSize(XMFLOAT2(100, 30));
	emissiveSlider->SetPos(XMFLOAT2(400, 300));
	emissiveSlider->OnSlide([=](wiEventArgs args) {
		material->emissive = args.fValue;
	});
	materialWindow->AddWidget(emissiveSlider);

	sssSlider = new wiSlider(0, 1, 0.0, 1000, "Subsurface Scattering: ");
	sssSlider->SetSize(XMFLOAT2(100, 30));
	sssSlider->SetPos(XMFLOAT2(400, 340));
	sssSlider->OnSlide([=](wiEventArgs args) {
		material->subsurfaceScattering = args.fValue;
	});
	materialWindow->AddWidget(sssSlider);




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
		wiRenderer::CleanUpStaticTemp();
		//wiRenderer::LoadModel("CONTENT/models/instanceBenchmark2/", "instanceBenchmark2");
		wiRenderer::LoadModel("CONTENT/models/cube/", "cube");
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

			if (pick.object != nullptr)
			{
				if (pick.subsetIndex < (int)pick.object->mesh->subsets.size())
				{
					material = pick.object->mesh->subsets[pick.subsetIndex].material;

					materialLabel->SetText("Material: " + material->name);
					waterCheckBox->SetCheck(material->water);
					normalMapSlider->SetValue(material->normalMapStrength);
					roughnessSlider->SetValue(material->roughness);
					reflectanceSlider->SetValue(material->reflectance);
					metalnessSlider->SetValue(material->metalness);
					alphaSlider->SetValue(material->alpha);
					refractionIndexSlider->SetValue(material->refractionIndex);
					emissiveSlider->SetValue(material->emissive);
					sssSlider->SetValue(material->subsurfaceScattering);
					materialWindow->SetEnabled(true);
				}

			}
			else
			{
				material = nullptr;
				materialLabel->SetText("Material: ");
				materialWindow->SetEnabled(false);
			}
		}

	}

	__super::Update();
}
void EditorComponent::Unload()
{
	// ...

	__super::Unload();
}
