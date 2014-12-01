#pragma once

#include "d3dStd.h"

//From Oculus SDK
enum TextureFormat
{
	Texture_RGBA            = 0x100,
    Texture_R               = 0x200,
	Texture_A				= 0x400,
	Texture_BGRA            = 0x800,
    Texture_DXT1            = 0x1100,
    Texture_DXT3            = 0x1200,
    Texture_DXT5            = 0x1300,
    Texture_Depth           = 0x8000,
    Texture_TypeMask        = 0xff00,
    Texture_Compressed      = 0x1000,
    Texture_SamplesMask     = 0x00ff,
	Texture_RenderTarget    = 0x10000,
	Texture_SampleDepth		= 0x20000,
    Texture_GenMipmaps      = 0x40000,
	Texture_KinectDynamic   = 0x80000,
};

class Texture
{
	friend class D3DRenderer;
	friend class TextureManager;
	friend class RenderTarget;
	friend class OVRManager;

public:
	~Texture();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	ID3D11Texture2D* getD3DTexture() const { return mpTexture; }
	ID3D11ShaderResourceView* getResourceView() const { return mpResourceView; }

private:
	Texture();

private:
	int mWidth;
	int mHeight;

	ID3D11Texture2D* mpTexture;
	ID3D11ShaderResourceView* mpResourceView;
};