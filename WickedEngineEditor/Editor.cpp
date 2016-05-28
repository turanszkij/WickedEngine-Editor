#include "stdafx.h"
#include "Editor.h"
#include "wiInitializer.h"
#include "wiRenderer.h"


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
}




void EditorComponent::Initialize()
{
	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	wiWindow* window = new wiWindow(&GetGUI(),"window");
	window->SetSize(XMFLOAT2(640, 480));
	GetGUI().AddWidget(window);

	wiLabel* label = new wiLabel("Label");
	label->SetPos(XMFLOAT2(100, 20));
	label->SetSize(XMFLOAT2(400, 20));
	window->AddWidget(label);

	wiButton* button = new wiButton("BUTTON");
	button->SetPos(XMFLOAT2(200, 200));
	button->OnClick([label](wiEventArgs args) {
		static int clicks = 0;
		clicks++;
		stringstream ss("");
		ss << "Button clicks: " << clicks;
		label->SetText(ss.str());
	});
	window->AddWidget(button);

	wiCheckBox* checkBox = new wiCheckBox("CHECKBOX");
	checkBox->SetPos(XMFLOAT2(200, 50));
	checkBox->OnClick([label](wiEventArgs args) {
		static int clicks = 0;
		clicks++;
		stringstream ss("");
		ss << "Checkbox value: " << args.bValue;
		label->SetText(ss.str());
	});
	window->AddWidget(checkBox);

	wiSlider* slider = new wiSlider(-100, 100, 0, 10000);
	slider->SetPos(XMFLOAT2(200, 80));
	slider->SetText("Slider");
	slider->OnSlide([window](wiEventArgs args) {
		//
	});
	window->AddWidget(slider);


	window->Scale(XMFLOAT3(1.3f, 0.8f, 1));
	window->Translate(XMFLOAT3(20, 20, 0));


	wiButton* buttonWnd1 = new wiButton("Wnd1");
	buttonWnd1->SetPos(XMFLOAT2(0, screenH - 40));
	buttonWnd1->SetSize(XMFLOAT2(100, 40));
	buttonWnd1->OnClick([window](wiEventArgs args) {
		window->SetVisible(!window->IsVisible());
	});
	GetGUI().AddWidget(buttonWnd1);

	wiButton* dragger = new wiButton("Dragger");
	dragger->SetPos(XMFLOAT2(500, 300));
	dragger->OnDrag([=](wiEventArgs args) {
		dragger->Translate(XMFLOAT3(args.deltaPos.x, args.deltaPos.y, 0));
	});
	GetGUI().AddWidget(dragger);

	DeferredRenderableComponent::Initialize();
}
void EditorComponent::Load()
{
	DeferredRenderableComponent::Load();

	// ...
}
void EditorComponent::Start()
{
	DeferredRenderableComponent::Start();
}
void EditorComponent::Unload()
{
	// ...

	DeferredRenderableComponent::Unload();
}
