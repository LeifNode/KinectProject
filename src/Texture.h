#pragma once

#include "d3dStd.h"

class Texture
{
	friend class D3DRenderer;
	friend class TextureManager;

private:
	Texture() {}
	~Texture() {}

private:
	ID3D11ShaderResourceView* mpTexture;
};