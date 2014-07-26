#pragma once

#include "d3dStd.h"
#include "Texture.h"

class GBuffer
{
	friend class D3DRenderer;

public:
	GBuffer();
	~GBuffer();

	void bindRenderTargets();

private:
	void Initialize(int width, int height);
	void DeInit();

	void OnResize(int width, int height);

private:
	Texture* mpDiffuseTexture;
	Texture* mpDepthStencilTexture;
	Texture* mpNormalTexture;
	Texture* mpSpecularTexture;
	Texture* mpEmissiveTexture;

	ID3D11DepthStencilView* mpDepthStencilTargetView;
	ID3D11RenderTargetView* mpDiffuseTargetView;
	ID3D11RenderTargetView* mpNormalTargetView;
	ID3D11RenderTargetView* mpSpecularTargetView;
	ID3D11RenderTargetView* mpEmissiveTargetView;

	ID3D11DepthStencilState* mpDepthStencilState;
};