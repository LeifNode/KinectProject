#pragma once

#include "d3dStd.h"
#include "LineRenderer.h"

class D3DRenderer;

class LeapRenderer
{
public:
	LeapRenderer();
	~LeapRenderer();

	void Initialize();

	void Update();
	void Render(D3DRenderer* renderer, int index);

private:
	void updateImages();
	void buildHandMasks();

private:
	Shader* mpImageShader;

	LineRenderer mLineRenderer;

	ID3D11Texture2D* mBrightnessTextures[2];
	ID3D11ShaderResourceView* mBrightnessResourceViews[2];

	ID3D11Texture2D* mDistortionTextures[2];
	ID3D11ShaderResourceView* mDistortionResourceViews[2];

	ID3D11Texture2D* mHandMaskTextures[2];
	ID3D11ShaderResourceView* mHandMaskRSV[2];

	ID3D11Buffer* mpLeapConstantBuffer;
};