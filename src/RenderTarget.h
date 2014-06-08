#pragma once

#include "d3dStd.h"
#include "Texture.h"

class RenderTarget
{
	friend class D3DRenderer;
	friend class OVRRenderer;

public:
	~RenderTarget();
	
	Texture* getRenderTargetTexture() const { return mpRenderTargetTexture; }

private:
	RenderTarget();

private:
	int mWidth;
	int mHeight;

	Texture* mpRenderTargetTexture;
	Texture* mpDepthTexture;

	ID3D11RenderTargetView* mpRenderTargetView;
	ID3D11DepthStencilView* mpDepthView;
};