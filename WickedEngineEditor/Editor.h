#pragma once
#include "WickedEngine.h"

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

	void Initialize() override;
	void Load() override;
	void Start() override;
	void Unload() override;
};

