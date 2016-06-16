#pragma once

struct Material;
class wiGUI;
class wiWindow;
class wiLabel;
class wiCheckBox;
class wiSlider;

class RendererWindow
{
public:
	RendererWindow(wiGUI* gui);
	~RendererWindow();

	wiGUI* GUI;

	wiWindow*	rendererWindow;
	wiCheckBox* vsyncCheckBox;
	wiCheckBox* partitionBoxesCheckBox;
	wiCheckBox* boneLinesCheckBox;
	wiCheckBox* envProbesCheckBox;
};

