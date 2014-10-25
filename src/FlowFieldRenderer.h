#pragma once

#include "d3dStd.h"
#include "Renderable.h"

class FlowFieldRenderer : public Renderable
{
public:
	FlowFieldRenderer(class FlowField* source);
	~FlowFieldRenderer();

	void Initialize();

	virtual void Render(D3DRenderer* renderer);

private:
	class FlowField* mpSource;

	class Shader* mpRenderShader;
};