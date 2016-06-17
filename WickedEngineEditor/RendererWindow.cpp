#include "stdafx.h"
#include "RendererWindow.h"


RendererWindow::RendererWindow(wiGUI* gui) : GUI(gui)
{
	assert(GUI && "Invalid GUI!");

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	rendererWindow = new wiWindow(GUI, "Renderer Window");
	rendererWindow->SetSize(XMFLOAT2(600, 400));
	rendererWindow->SetEnabled(true);
	GUI->AddWidget(rendererWindow);

	float x = 250, y = 0, step = 30;

	vsyncCheckBox = new wiCheckBox("VSync: ");
	vsyncCheckBox->SetPos(XMFLOAT2(x, y += step));
	vsyncCheckBox->OnClick([](wiEventArgs args) {
		wiRenderer::GetDevice()->SetVSyncEnabled(args.bValue);
	});
	vsyncCheckBox->SetCheck(wiRenderer::GetDevice()->GetVSyncEnabled());
	rendererWindow->AddWidget(vsyncCheckBox);

	partitionBoxesCheckBox = new wiCheckBox("SPTree visualizer: ");
	partitionBoxesCheckBox->SetPos(XMFLOAT2(x, y += step));
	partitionBoxesCheckBox->OnClick([](wiEventArgs args) {
		wiRenderer::SetToDrawDebugBoxes(args.bValue);
	});
	partitionBoxesCheckBox->SetCheck(wiRenderer::GetToDrawDebugBoxes());
	rendererWindow->AddWidget(partitionBoxesCheckBox);

	boneLinesCheckBox = new wiCheckBox("Bone line visualizer: ");
	boneLinesCheckBox->SetPos(XMFLOAT2(x, y += step));
	boneLinesCheckBox->OnClick([](wiEventArgs args) {
		wiRenderer::SetToDrawDebugBoneLines(args.bValue);
	});
	boneLinesCheckBox->SetCheck(wiRenderer::GetToDrawDebugBoneLines());
	rendererWindow->AddWidget(boneLinesCheckBox);

	envProbesCheckBox = new wiCheckBox("Env probe visualizer: ");
	envProbesCheckBox->SetPos(XMFLOAT2(x, y += step));
	envProbesCheckBox->OnClick([](wiEventArgs args) {
		wiRenderer::SetToDrawDebugEnvProbes(args.bValue);
	});
	envProbesCheckBox->SetCheck(wiRenderer::GetToDrawDebugEnvProbes());
	rendererWindow->AddWidget(envProbesCheckBox);




	rendererWindow->Translate(XMFLOAT3(30, 30, 0));
	rendererWindow->SetVisible(false);
}


RendererWindow::~RendererWindow()
{
	SAFE_DELETE(rendererWindow);
	SAFE_DELETE(vsyncCheckBox);
	SAFE_DELETE(partitionBoxesCheckBox);
	SAFE_DELETE(boneLinesCheckBox);
	SAFE_DELETE(envProbesCheckBox);
}
