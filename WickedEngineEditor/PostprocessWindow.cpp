#include "stdafx.h"
#include "PostprocessWindow.h"


PostprocessWindow::PostprocessWindow(Renderable3DComponent* comp) : component(comp)
{
	assert(component && "PostprocessWnd invalid component!");

	GUI = &component->GetGUI();

	assert(GUI && "Invalid GUI!");

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	ppWindow = new wiWindow(GUI, "PostProcess Window");
	ppWindow->SetSize(XMFLOAT2(300, 340));
	GUI->AddWidget(ppWindow);

	float x = 200;
	float y = 0;

	lensFlareCheckBox = new wiCheckBox("LensFlare: ");
	lensFlareCheckBox->SetPos(XMFLOAT2(x, y += 35));
	lensFlareCheckBox->SetCheck(component->getLensFlareEnabled());
	lensFlareCheckBox->OnClick([&](wiEventArgs args) {
		component->setLensFlareEnabled(args.bValue);
	});
	ppWindow->AddWidget(lensFlareCheckBox);

	lightShaftsCheckBox = new wiCheckBox("LightShafts: ");
	lightShaftsCheckBox->SetPos(XMFLOAT2(x, y += 35));
	lightShaftsCheckBox->SetCheck(component->getLightShaftsEnabled());
	lightShaftsCheckBox->OnClick([&](wiEventArgs args) {
		component->setLightShaftsEnabled(args.bValue);
	});
	ppWindow->AddWidget(lightShaftsCheckBox);

	ssaoCheckBox = new wiCheckBox("SSAO: ");
	ssaoCheckBox->SetPos(XMFLOAT2(x, y += 35));
	ssaoCheckBox->SetCheck(component->getSSAOEnabled());
	ssaoCheckBox->OnClick([&](wiEventArgs args) {
		component->setSSAOEnabled(args.bValue);
	});
	ppWindow->AddWidget(ssaoCheckBox);

	ssrCheckBox = new wiCheckBox("SSR: ");
	ssrCheckBox->SetPos(XMFLOAT2(x, y += 35));
	ssrCheckBox->SetCheck(component->getSSREnabled());
	ssrCheckBox->OnClick([&](wiEventArgs args) {
		component->setSSREnabled(args.bValue);
	});
	ppWindow->AddWidget(ssrCheckBox);

	sssCheckBox = new wiCheckBox("SSS: ");
	sssCheckBox->SetPos(XMFLOAT2(x, y += 35));
	sssCheckBox->SetCheck(component->getSSSEnabled());
	sssCheckBox->OnClick([&](wiEventArgs args) {
		component->setSSSEnabled(args.bValue);
	});
	ppWindow->AddWidget(sssCheckBox);

	eyeAdaptionCheckBox = new wiCheckBox("EyeAdaption: ");
	eyeAdaptionCheckBox->SetPos(XMFLOAT2(x, y += 35));
	eyeAdaptionCheckBox->SetCheck(component->getEyeAdaptionEnabled());
	eyeAdaptionCheckBox->OnClick([&](wiEventArgs args) {
		component->setEyeAdaptionEnabled(args.bValue);
	});
	ppWindow->AddWidget(eyeAdaptionCheckBox);

	motionBlurCheckBox = new wiCheckBox("MotionBlur: ");
	motionBlurCheckBox->SetPos(XMFLOAT2(x, y += 35));
	motionBlurCheckBox->SetCheck(component->getMotionBlurEnabled());
	motionBlurCheckBox->OnClick([&](wiEventArgs args) {
		component->setMotionBlurEnabled(args.bValue);
	});
	ppWindow->AddWidget(motionBlurCheckBox);

	depthOfFieldCheckBox = new wiCheckBox("DepthOfField: ");
	depthOfFieldCheckBox->SetPos(XMFLOAT2(x, y += 35));
	depthOfFieldCheckBox->SetCheck(component->getDepthOfFieldEnabled());
	depthOfFieldCheckBox->OnClick([&](wiEventArgs args) {
		component->setDepthOfFieldEnabled(args.bValue);
	});
	ppWindow->AddWidget(depthOfFieldCheckBox);


	ppWindow->Translate(XMFLOAT3(screenW - 320, 50, 0));
	ppWindow->SetVisible(false);

}


PostprocessWindow::~PostprocessWindow()
{
	SAFE_DELETE(ppWindow);
	SAFE_DELETE(lensFlareCheckBox);
	SAFE_DELETE(lightShaftsCheckBox);
	SAFE_DELETE(ssaoCheckBox);
	SAFE_DELETE(ssrCheckBox);
	SAFE_DELETE(sssCheckBox);
	SAFE_DELETE(eyeAdaptionCheckBox);
}
