#pragma once

#include "d3dStd.h"
#include "Renderable.h"

class FlowFieldRenderer : public Renderable
{
public:
	FlowFieldRenderer(class FlowField* source);
	FlowFieldRenderer(ID3D11ShaderResourceView* source);
	~FlowFieldRenderer();

	void Initialize();

	virtual void Render(D3DRenderer* renderer);

private:
	class FlowField* mpSource;
	ID3D11ShaderResourceView* mpInternalSource;

	class Shader* mpRenderShader;
};