#pragma once

#include "d3dStd.h"
#include "Shader.h"

class D3DRenderer;

class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void Initialize();

	void OnResize();

	void Render(D3DRenderer* renderer);

private:
	void renderDirectionalLight(D3DRenderer* renderer);

private:
	Shader* mpDirectionalLightPass;
};