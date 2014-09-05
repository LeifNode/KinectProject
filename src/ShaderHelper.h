#pragma once

#include "d3dStd.h"
#include "Texture.h"

class ShaderHelper
{
public:
	static ID3D11ShaderResourceView* createRandomTexture();
};