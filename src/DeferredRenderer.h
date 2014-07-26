#pragma once

#include "d3dStd.h"
#include "Texture.h"

class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void Initialize();

	void OnResize();

	void bindRenderTargets();

private:
	
};