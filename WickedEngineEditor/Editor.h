#pragma once
#include "WickedEngine.h"

class MaterialWindow;
class PostprocessWindow;
class WorldWindow;
class ObjectWindow;
class MeshWindow;

class Editor : public MainComponent
{
public:
	Editor();
	~Editor();

	void Initialize();
};

class EditorComponent : public DeferredRenderableComponent
{
public:
	MaterialWindow*			materialWnd;
	PostprocessWindow*		postprocessWnd;
	WorldWindow*			worldWnd;
	ObjectWindow*			objectWnd;
	MeshWindow*				meshWnd;

	void Initialize() override;
	void Load() override;
	void Start() override;
	void Update() override;
	void Unload() override;
};

