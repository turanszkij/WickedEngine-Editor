#include "stdafx.h"
#include "MaterialWindow.h"


MaterialWindow::MaterialWindow(wiGUI* gui) : GUI(gui)
{
	assert(GUI && "Invalid GUI!");

	material = nullptr;

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	materialWindow = new wiWindow(GUI, "Material Window");
	materialWindow->SetSize(XMFLOAT2(600, 400));
	materialWindow->SetEnabled(false);
	GUI->AddWidget(materialWindow);

	materialLabel = new wiLabel("MaterialName");
	materialLabel->SetPos(XMFLOAT2(10, 30));
	materialLabel->SetSize(XMFLOAT2(300, 20));
	materialLabel->SetText("");
	materialWindow->AddWidget(materialLabel);

	waterCheckBox = new wiCheckBox("Water: ");
	waterCheckBox->SetPos(XMFLOAT2(470, 30));
	waterCheckBox->OnClick([&](wiEventArgs args) {
		material->water = args.bValue;
	});
	materialWindow->AddWidget(waterCheckBox);

	normalMapSlider = new wiSlider(0, 4, 1, 4000, "Normalmap: ");
	normalMapSlider->SetSize(XMFLOAT2(100, 30));
	normalMapSlider->SetPos(XMFLOAT2(400, 60));
	normalMapSlider->OnSlide([&](wiEventArgs args) {
		material->normalMapStrength = args.fValue;
	});
	materialWindow->AddWidget(normalMapSlider);

	roughnessSlider = new wiSlider(0, 1, 0.5, 1000, "Roughness: ");
	roughnessSlider->SetSize(XMFLOAT2(100, 30));
	roughnessSlider->SetPos(XMFLOAT2(400, 100));
	roughnessSlider->OnSlide([&](wiEventArgs args) {
		material->roughness = args.fValue;
	});
	materialWindow->AddWidget(roughnessSlider);

	reflectanceSlider = new wiSlider(0, 1, 0.5, 1000, "Reflectance: ");
	reflectanceSlider->SetSize(XMFLOAT2(100, 30));
	reflectanceSlider->SetPos(XMFLOAT2(400, 140));
	reflectanceSlider->OnSlide([&](wiEventArgs args) {
		material->reflectance = args.fValue;
	});
	materialWindow->AddWidget(reflectanceSlider);

	metalnessSlider = new wiSlider(0, 1, 0.0, 1000, "Metalness: ");
	metalnessSlider->SetSize(XMFLOAT2(100, 30));
	metalnessSlider->SetPos(XMFLOAT2(400, 180));
	metalnessSlider->OnSlide([&](wiEventArgs args) {
		material->metalness = args.fValue;
	});
	materialWindow->AddWidget(metalnessSlider);

	alphaSlider = new wiSlider(0, 1, 1.0, 1000, "Alpha: ");
	alphaSlider->SetSize(XMFLOAT2(100, 30));
	alphaSlider->SetPos(XMFLOAT2(400, 220));
	alphaSlider->OnSlide([&](wiEventArgs args) {
		material->alpha = args.fValue;
	});
	materialWindow->AddWidget(alphaSlider);

	refractionIndexSlider = new wiSlider(0, 1.0, 0.02, 1000, "Refraction Index: ");
	refractionIndexSlider->SetSize(XMFLOAT2(100, 30));
	refractionIndexSlider->SetPos(XMFLOAT2(400, 260));
	refractionIndexSlider->OnSlide([&](wiEventArgs args) {
		material->refractionIndex = args.fValue;
	});
	materialWindow->AddWidget(refractionIndexSlider);

	emissiveSlider = new wiSlider(0, 1, 0.0, 1000, "Emissive: ");
	emissiveSlider->SetSize(XMFLOAT2(100, 30));
	emissiveSlider->SetPos(XMFLOAT2(400, 300));
	emissiveSlider->OnSlide([&](wiEventArgs args) {
		material->emissive = args.fValue;
	});
	materialWindow->AddWidget(emissiveSlider);

	sssSlider = new wiSlider(0, 1, 0.0, 1000, "Subsurface Scattering: ");
	sssSlider->SetSize(XMFLOAT2(100, 30));
	sssSlider->SetPos(XMFLOAT2(400, 340));
	sssSlider->OnSlide([&](wiEventArgs args) {
		material->subsurfaceScattering = args.fValue;
	});
	materialWindow->AddWidget(sssSlider);





	materialWindow->Translate(XMFLOAT3(30, 30, 0));
	materialWindow->SetVisible(false);

}

MaterialWindow::~MaterialWindow()
{
	SAFE_DELETE(materialWindow);
	SAFE_DELETE(materialLabel);
	SAFE_DELETE(waterCheckBox);
	SAFE_DELETE(normalMapSlider);
	SAFE_DELETE(roughnessSlider);
	SAFE_DELETE(reflectanceSlider);
	SAFE_DELETE(metalnessSlider);
	SAFE_DELETE(alphaSlider);
	SAFE_DELETE(refractionIndexSlider);
	SAFE_DELETE(emissiveSlider);
	SAFE_DELETE(sssSlider);
}



void MaterialWindow::SetMaterial(Material* mat)
{
	material = mat;
	if (material != nullptr)
	{
		materialLabel->SetText(material->name);
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
	else
	{
		materialLabel->SetText("No material selected");
		materialWindow->SetEnabled(false);
	}
}
