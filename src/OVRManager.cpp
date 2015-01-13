#include "OVRManager.h"
#include "RenderTarget.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "Camera.h"

OVRManager::OVRManager()
	:mpRenderTarget(NULL),
	mDeviceConnected(false),
	mEyeHeight(OVR_DEFAULT_EYE_HEIGHT),
	mIPD(OVR_DEFAULT_IPD)
{
	mInitialized = (bool)ovr_Initialize();
}

OVRManager::~OVRManager()
{
	SAFE_DELETE(mpRenderTarget);

	ovrHmd_Destroy(mHMD);
	ovr_Shutdown();
}

void OVRManager::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();
	
	if (mInitialized)
	{
		mHMD = ovrHmd_Create(0);
		if (!mHMD || mHMD->ProductName[0] == '\0')
		{
			//MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK);
			mDeviceConnected = false;
			return;
		}
	}
	else
		return;

	ovrHmd_AttachToWindow(mHMD, gpApplication->mainWnd(), NULL, NULL);

	OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(mHMD, ovrEye_Left,  mHMD->DefaultEyeFov[0], 1.0f);
    OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(mHMD, ovrEye_Right, mHMD->DefaultEyeFov[1], 1.0f);

    mRenderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
    mRenderTargetSize.h = max ( recommenedTex0Size.h, recommenedTex1Size.h ); 

	mpRenderTarget = renderer->createRenderTarget(mRenderTargetSize.w, mRenderTargetSize.h); 

	ovrFovPort eyeFov[2] = { mHMD->DefaultEyeFov[0], mHMD->DefaultEyeFov[1] } ;

	mEyeRenderViewport[0].Pos  = OVR::Vector2i(0,0);
	mEyeRenderViewport[0].Size = OVR::Sizei(mRenderTargetSize.w / 2, mRenderTargetSize.h);
	mEyeRenderViewport[1].Pos  = OVR::Vector2i((mRenderTargetSize.w + 1) / 2, 0);
	mEyeRenderViewport[1].Size = mEyeRenderViewport[0].Size;

	mEyeTextures[0].D3D11.Header.API            = ovrRenderAPI_D3D11;
    mEyeTextures[0].D3D11.Header.TextureSize    = mRenderTargetSize;
	mEyeTextures[0].D3D11.Header.RenderViewport = mEyeRenderViewport[0];
	mEyeTextures[0].D3D11.pTexture              = mpRenderTarget->getRenderTargetTexture()->mpTexture;
	mEyeTextures[0].D3D11.pSRView               = mpRenderTarget->getRenderTargetTexture()->mpResourceView;

	mEyeTextures[1] = mEyeTextures[0];
    mEyeTextures[1].D3D11.Header.RenderViewport = mEyeRenderViewport[1];

	//Configure d3d11
	ovrD3D11Config d3d11cfg;
    d3d11cfg.D3D11.Header.API         = ovrRenderAPI_D3D11;
	d3d11cfg.D3D11.Header.RTSize      = OVR::Sizei(mHMD->Resolution.w, mHMD->Resolution.h);
    d3d11cfg.D3D11.Header.Multisample = 1;
	d3d11cfg.D3D11.pDevice            = renderer->device();
	d3d11cfg.D3D11.pDeviceContext     = renderer->context();
	d3d11cfg.D3D11.pBackBufferRT      = renderer->renderTarget();
	d3d11cfg.D3D11.pSwapChain         = renderer->swapChain();

	if (!ovrHmd_ConfigureRendering(mHMD, &d3d11cfg.Config,
		                           ovrDistortionCap_Chromatic | 
								   ovrDistortionCap_TimeWarp | 
								   ovrDistortionCap_Overdrive,
								   eyeFov, mEyeRenderDesc)) return;


	ovrHmd_SetEnabledCaps(mHMD, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

	if (!ovrHmd_ConfigureTracking(mHMD, ovrTrackingCap_Orientation |
									   ovrTrackingCap_MagYawCorrection |
									   ovrTrackingCap_Position, 0)) return;

	mEyeHeight = ovrHmd_GetFloat(mHMD, OVR_KEY_EYE_HEIGHT, OVR_DEFAULT_EYE_HEIGHT);
	mIPD = ovrHmd_GetFloat(mHMD, OVR_KEY_IPD, OVR_DEFAULT_IPD);

	mDeviceConnected = true;
}

void OVRManager::OnResize()
{
	if (mDeviceConnected)
	{
		D3DRenderer* renderer = gpApplication->getRenderer();

		ovrFovPort eyeFov[2] = { mHMD->DefaultEyeFov[0], mHMD->DefaultEyeFov[1] } ;

		ovrD3D11Config d3d11cfg;
		d3d11cfg.D3D11.Header.API         = ovrRenderAPI_D3D11;
		d3d11cfg.D3D11.Header.RTSize      = OVR::Sizei(mHMD->Resolution.w, mHMD->Resolution.h);
		d3d11cfg.D3D11.Header.Multisample = 1;
		d3d11cfg.D3D11.pDevice            = renderer->device();
		d3d11cfg.D3D11.pDeviceContext     = renderer->context();
		d3d11cfg.D3D11.pBackBufferRT      = renderer->renderTarget();
		d3d11cfg.D3D11.pSwapChain         = renderer->swapChain();

		ovrHmd_ConfigureRendering(mHMD, &d3d11cfg.Config,
									   ovrDistortionCap_Chromatic | 
									   ovrDistortionCap_TimeWarp | 
									   ovrDistortionCap_Overdrive,
									   eyeFov, mEyeRenderDesc);
	}
}

XMMATRIX OVRManager::getProjection(int eyeIndex) const
{
	return XMMatrixTranspose(XMLoadFloat4x4(&XMFLOAT4X4(&ovrMatrix4f_Projection(mEyeRenderDesc[mHMD->EyeRenderOrder[eyeIndex]].Fov, 0.01f, 10000.0f, true).M[0][0])));
}

XMMATRIX OVRManager::getOffsetView(const Camera& camera, int eyeIndex, float IPDScaling) const
{
	XMVECTOR position = XMLoadFloat3(&camera.getPosition());
	XMVECTOR forward = XMVector3Rotate(XMLoadFloat3(&camera.getDirection()), mHeadOrientation);
	XMVECTOR up = XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), mHeadOrientation);
	//XMVECTOR right = XMVector3Cross(

	return XMMatrixIdentity();
}

XMVECTOR OVRManager::getEyeOffset(int eyeIndex)
{
	return XMVectorSet(mIPD * 0.5f * (eyeIndex == 0 ? -1.0f : 1.0f), 0.0f, 0.00f, 0.0f);
	//return XMVectorSet(-mEyeRenderPoses[eyeIndex].Position.x, mEyeRenderPoses[eyeIndex].Position.y, -mEyeRenderPoses[eyeIndex].Position.z, 0.0f);
}

void OVRManager::Update(float dt)
{

}

void OVRManager::UpdateTrackingState()
{
	if (IsDeviceConnected())
	{
		D3DRenderer* renderer = gpApplication->getRenderer();

		ovrVector3f hmdToEyeViewOffset[2] = { mEyeRenderDesc[0].HmdToEyeViewOffset, mEyeRenderDesc[1].HmdToEyeViewOffset };
		ovrHmd_GetEyePoses(mHMD, 0, hmdToEyeViewOffset, mEyeRenderPoses, &mHmdState);

		XMVECTOR rotQuat = XMLoadFloat4(&XMFLOAT4(mHmdState.HeadPose.ThePose.Orientation.x,
												  mHmdState.HeadPose.ThePose.Orientation.y,
												  mHmdState.HeadPose.ThePose.Orientation.z,
												  mHmdState.HeadPose.ThePose.Orientation.w));
		
		XMVECTOR axis;
		float angle;
	
		XMQuaternionToAxisAngle(&axis, &angle, rotQuat);

		axis = XMVectorSet(XMVectorGetX(axis), -XMVectorGetY(axis), XMVectorGetZ(axis), 0.0f);

		rotQuat = XMQuaternionRotationAxis(axis, -angle);

		mHeadPosition = XMVectorSet(-mHmdState.HeadPose.ThePose.Position.x, mHmdState.HeadPose.ThePose.Position.y, -mHmdState.HeadPose.ThePose.Position.z, 0.0f);
		mHeadOrientation = rotQuat;
	}
	else
	{
		mHeadPosition = XMVectorZero();
		mHeadOrientation = XMQuaternionIdentity();
	}
}

void OVRManager::ClearRenderTarget()
{
	//gpApplication->getRenderer()->clear(mpRenderTarget);
}

void OVRManager::PreRender(int eyeIndex)
{
	if (IsDeviceConnected())
	{
		ovrEyeType eye = mHMD->EyeRenderOrder[eyeIndex];

		gpApplication->getRenderer()->setRenderTarget(mpRenderTarget);
		//renderer->getGBuffer()->bindRenderTargets();

		gpApplication->getRenderer()->setViewport(mEyeRenderViewport[eye].Size.w, mEyeRenderViewport[eye].Size.h, mEyeRenderViewport[eye].Pos.x, mEyeRenderViewport[eye].Pos.y); 
	}
}

void OVRManager::Render(D3DRenderer* renderer)
{

}

void OVRManager::PostRender()
{
	if (IsDeviceConnected())
	{
		D3DRenderer* renderer = gpApplication->getRenderer();

		//renderer->setBlendState(false);
		//renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);

		int width = gpApplication->getClientWidth();
		int height = gpApplication->getClientHeight();

		renderer->setViewport(width, height, 0, 0);

		ovrHmd_EndFrame(mHMD, mEyeRenderPoses, &mEyeTextures[0].Texture);
	}
}

void OVRManager::BeginFrame()
{
	if (IsDeviceConnected())
	{
		gpApplication->getRenderer()->clear(mpRenderTarget);
		ovrHmd_BeginFrame(mHMD, 0); 
	}
}

void OVRManager::EndFrame()
{
}