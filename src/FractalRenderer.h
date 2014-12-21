#pragma once

#include "d3dStd.h"
#include "Renderable.h"

class FractalRenderer : public Renderable
{
public:
	FractalRenderer();
	~FractalRenderer();

	void Initialize();

	void Render(class D3DRenderer* renderer);

private:
	class Shader* mpFractalShader;
};