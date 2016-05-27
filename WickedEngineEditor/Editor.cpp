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
	wiLabel* label = new wiLabel("Label");
	label->SetPos(XMFLOAT2(100, 20));
	label->SetSize(XMFLOAT2(400, 20));
	GetGUI().AddWidget(label);

	wiButton* button = new wiButton("BUTTON");
	button->SetPos(XMFLOAT2(200, 200));
	button->OnClick([label](wiEventArgs args) {
		static int clicks = 0;
		clicks++;
		stringstream ss("");
		ss << "Button clicks: " << clicks;
		label->SetText(ss.str());
	});
	GetGUI().AddWidget(button);

	wiCheckBox* checkBox = new wiCheckBox("CHECKBOX");
	checkBox->SetPos(XMFLOAT2(200, 50));
	checkBox->OnClick([label](wiEventArgs args) {
		static int clicks = 0;
		clicks++;
		stringstream ss("");
		ss << "Checkbox value: " << args.bValue;
		label->SetText(ss.str());
	});
	GetGUI().AddWidget(checkBox);

	wiSlider* slider = new wiSlider(1, 100, 10, 99);
	slider->SetPos(XMFLOAT2(200, 80));
	slider->SetText("Slider");
	GetGUI().AddWidget(slider);

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
