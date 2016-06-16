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

	vsyncCheckBox = new wiCheckBox("VSync: ");
	vsyncCheckBox->SetPos(XMFLOAT2(470, 30));
	vsyncCheckBox->OnClick([](wiEventArgs args) {
		wiRenderer::GetDevice()->SetVSyncEnabled(args.bValue);
	});
	vsyncCheckBox->SetCheck(wiRenderer::GetDevice()->GetVSyncEnabled());
	rendererWindow->AddWidget(vsyncCheckBox);




	rendererWindow->Translate(XMFLOAT3(30, 30, 0));
	rendererWindow->SetVisible(false);
}


RendererWindow::~RendererWindow()
{
}
