#pragma once

#define USE_RIFT 1

#include "d3dStd.h"
#define OVR_D3D_VERSION 11
#include <ovr/OVR.h>
#include <ovr/OVR_CAPI_D3D.h>

class D3DRenderer;
class Texture;
class RenderTarget;
class Camera;

class OVRManager
{
public:
	OVRManager();
	~OVRManager();

	void Initialize();
	void OnResize();

	void Update(float dt);

	void UpdateTrackingState();

	void ClearRenderTarget();

	void PreRender(int eyeIndex);
	void Render(D3DRenderer* renderer);
	void PostRender();

	void BeginFrame();
	void EndFrame();

	XMVECTOR getHeadOrientation() const { return mHeadOrientation; }
	XMVECTOR getHeadPosition() const { return mHeadPosition; }

	bool IsDeviceConnected() const { return mDeviceConnected; }
	OVR::Sizei getRenderTargetSize() const { return mRenderTargetSize; }
	float getEyeHeight() const { return mEyeHeight; }

	XMMATRIX getProjection(int eyeIndex) const;
	XMMATRIX getOffsetView(const Camera& camera, int eyeIndex, float IPDScaling = 1.0f) const;

	XMVECTOR getEyeOffset(int eyeIndex);

private:
	ovrHmd mHMD;
	ovrTrackingState mHmdState;

	XMVECTOR mHeadOrientation;
	XMVECTOR mHeadPosition;

	ovrPosef mEyeRenderPoses[2];
	ovrEyeRenderDesc mEyeRenderDesc[2];
	ovrD3D11Texture mEyeTextures[2];
	ovrRecti mEyeRenderViewport[2];
	OVR::Sizei mRenderTargetSize;
	RenderTarget* mpRenderTarget;
	
	ovrFrameTiming mTimer;

	float mEyeHeight;
	float mIPD;
	bool mDeviceConnected;
	bool mInitialized;
};