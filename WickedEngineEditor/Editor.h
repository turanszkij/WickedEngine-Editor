#pragma once
#include "WickedEngine.h"

class MaterialWindow;
class PostprocessWindow;
class WorldWindow;
class ObjectWindow;
class MeshWindow;
class CameraWindow;

class EditorLoadingScreen : public LoadingScreenComponent
{
public:
	void Compose() override;
};

class Editor;
class EditorComponent : public DeferredRenderableComponent
{
public:
	MaterialWindow*			materialWnd;
	PostprocessWindow*		postprocessWnd;
	WorldWindow*			worldWnd;
	ObjectWindow*			objectWnd;
	MeshWindow*				meshWnd;
	CameraWindow*			cameraWnd;

	Editor*					main;
	EditorLoadingScreen*	loader;

	void Initialize() override;
	void Load() override;
	void Start() override;
	void Update() override;
	void Unload() override;
};

class Editor : public MainComponent
{
public:
	Editor();
	~Editor();

	EditorComponent*		renderComponent;
	EditorLoadingScreen*	loader;

	void Initialize();
};

