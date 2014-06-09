#pragma once

#include "d3dStd.h"
#include <NuiApi.h>
#include "Texture.h"

class D3DRenderer;
class Shader;

struct CBKinectInfo
{
	XMFLOAT4 XYScale;
};

class KinectRenderer
{
	static const NUI_IMAGE_RESOLUTION NUI_COLOR_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;
	static const NUI_IMAGE_RESOLUTION NUI_DEPTH_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

public:
	KinectRenderer();
	~KinectRenderer();

	void Initialize();

	void Update(float dt);
	void Render(D3DRenderer* renderer);

private:
	HRESULT ProcessDepth();
	HRESULT ProcessColor();
	HRESULT MapColorToDepth();

private:
	Shader* mpKinectRenderShader;

	ID3D11Buffer* mpVertexBuffer;
	ID3D11Buffer* mpKinectConstantBuffer;

	INuiSensor* mpNuiSensor;

	HANDLE mhNextDepthFrameEvent;
	HANDLE mpDepthStreamHandle;
	HANDLE mhNextColorFrameEvent;
	HANDLE mpColorStreamHandle;

	Texture* mpColorTexture;
	Texture* mpDepthTexture;

	LONG mDepthWidth;
	LONG mDepthHeight;
	LONG mColorWidth;
	LONG mColorHeight;

	USHORT* mDepthD16;
    BYTE* mColorRGBX;
    LONG* mColorCoordinates;

	bool mDepthReceived;
	bool mColorReceived;

	float mxyScale;
};