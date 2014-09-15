#pragma once

#define USE_RIFT 0
#define OVR_D3D_VERSION 11

#include "d3dStd.h"
#include <ovr/OVR.h>
#include <ovr/OVR_CAPI_D3D.h>

class D3DRenderer;
class Texture;
class RenderTarget;

const unsigned DistortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

class OVRRenderer
{
public:
	OVRRenderer();
	~OVRRenderer();

	void Initialize();
	void OnResize();

	void Update(float dt);

	void PreRender(int eyeIndex);
	void Render(D3DRenderer* renderer);
	void PostRender(int eyeIndex);

	void EndFrame();

	ovrPosef mEyeRenderPoses[2];
	XMVECTOR mEyeOrientations[2];
	XMMATRIX getProjection(int eyeIndex);

private:
	ovrHmd mHMD;
	ovrHmdDesc mHMDDesc;
	ovrEyeRenderDesc mEyeRenderDesc[2];
	ovrD3D11Texture mEyeTextures[2];
	ovrRecti mEyeRenderViewport[2];
	RenderTarget* mpRenderTarget;
	
	ovrFrameTiming mTimer;
};