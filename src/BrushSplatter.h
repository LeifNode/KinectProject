#pragma once

#include "d3dStd.h"

//Splats brush texture to surface

class Texture;

class BrushSplatter
{
public:
	BrushSplatter();
	~BrushSplatter();

	void Initialize();

	void Update(float dt);

	ID3D11Texture2D* getIntermediateSurfaceResource() const { return mpIntermediateSurfaceTexture; }
	ID3D11ShaderResourceView* getIntermediateSurfaceSRV() const { return mpIntermediateSurfaceSRV; }

private:
	void initializeRenderTarget();
	void initializeShaders();

private:
	std::map<std::string, Texture*> mBrushes;
	std::map<std::string, class Shader*> mBrushShaders;

	ID3D11Texture2D* mpIntermediateSurfaceTexture;
	ID3D11ShaderResourceView* mpIntermediateSurfaceSRV;
	ID3D11RenderTargetView* mpIntermediateSurfaceRTV;
};