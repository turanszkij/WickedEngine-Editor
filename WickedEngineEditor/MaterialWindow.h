#pragma once

struct Material;
class wiGUI;
class wiWindow;
class wiLabel;
class wiCheckBox;
class wiSlider;

class MaterialWindow
{
public:
	MaterialWindow(wiGUI* gui);
	~MaterialWindow();

	void SetMaterial(Material* mat);

	wiGUI* GUI;

	Material* material;

	wiWindow*	materialWindow;
	wiLabel*	materialLabel;
	wiCheckBox* waterCheckBox;
	wiSlider*	normalMapSlider;
	wiSlider*	roughnessSlider;
	wiSlider*	reflectanceSlider;
	wiSlider*	metalnessSlider;
	wiSlider*	alphaSlider;
	wiSlider*	refractionIndexSlider;
	wiSlider*	emissiveSlider;
	wiSlider*	sssSlider;
};

