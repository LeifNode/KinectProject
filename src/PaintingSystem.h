#pragma once

#include "d3dStd.h"

class PaintingSystem
{
public:
	PaintingSystem();
	~PaintingSystem();

	void Initialize();

	void Update(float dt);

	void Render(class D3DRenderer* renderer);

private:
	void initializeCompositeSurface();

private:
	class FlowField* mpFlowField;
	class FlowFieldRenderer* mpFlowFieldRenderer;
	class BrushSplatter* mpBrushSplatter;

	ID3D11Texture2D* mpCompositedSurfaceTexture;
	ID3D11ShaderResourceView* mpCompositedSurfaceSRV;
	ID3D11RenderTargetView* mpCompositedSurfaceRTV;
};