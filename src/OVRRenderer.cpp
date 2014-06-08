#include "OVRRenderer.h"
#include "RenderTarget.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

OVRRenderer::OVRRenderer()
	:mpRenderTarget(NULL)
{

}

OVRRenderer::~OVRRenderer()
{
	SAFE_DELETE(mpRenderTarget);

	ovrHmd_Destroy(mHMD);
	ovr_Shutdown();
}

void OVRRenderer::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	ovr_Initialize();

	mHMD = ovrHmd_Create(0);
	if (!mHMD)
    {
        MessageBoxA(NULL,"Oculus Rift not detected.","", MB_OK);
		return;
    }

	ovrHmd_GetDesc(mHMD, &mHMDDesc);
	if (mHMDDesc.DisplayDeviceName[0] == '\0')
        //MessageBoxA(NULL,"Rift detected, display not enabled.","", MB_OK);
		std::cout << "Rift detected, display not enabled.\n";

	OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(mHMD, ovrEye_Left,  mHMDDesc.DefaultEyeFov[0], 1.0f);
    OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(mHMD, ovrEye_Right, mHMDDesc.DefaultEyeFov[1], 1.0f);
	OVR::Sizei RenderTargetSize;
    RenderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
    RenderTargetSize.h = max ( recommenedTex0Size.h, recommenedTex1Size.h ); 

	mpRenderTarget = renderer->createRenderTarget(RenderTargetSize.w, RenderTargetSize.h);

	ovrFovPort eyeFov[2] = { mHMDDesc.DefaultEyeFov[0], mHMDDesc.DefaultEyeFov[1] } ;

	mEyeRenderViewport[0].Pos  = OVR::Vector2i(0,0);
	mEyeRenderViewport[0].Size = OVR::Sizei(RenderTargetSize.w / 2, RenderTargetSize.h);
	mEyeRenderViewport[1].Pos  = OVR::Vector2i((RenderTargetSize.w + 1) / 2, 0);
	mEyeRenderViewport[1].Size = mEyeRenderViewport[0].Size;

	mEyeTextures[0].D3D11.Header.API            = ovrRenderAPI_D3D11;
    mEyeTextures[0].D3D11.Header.TextureSize    = RenderTargetSize;
	mEyeTextures[0].D3D11.Header.RenderViewport = mEyeRenderViewport[0];
	mEyeTextures[0].D3D11.pTexture              = mpRenderTarget->getRenderTargetTexture()->mpTexture;
	mEyeTextures[0].D3D11.pSRView               = mpRenderTarget->getRenderTargetTexture()->mpResourceView;

	mEyeTextures[1] = mEyeTextures[0];
    mEyeTextures[1].D3D11.Header.RenderViewport = mEyeRenderViewport[1];

	//Configure d3d11
	ovrD3D11Config d3d11cfg;
    d3d11cfg.D3D11.Header.API         = ovrRenderAPI_D3D11;
	d3d11cfg.D3D11.Header.RTSize      = OVR::Sizei(mHMDDesc.Resolution.w, mHMDDesc.Resolution.h);
    d3d11cfg.D3D11.Header.Multisample = 1;
	d3d11cfg.D3D11.pDevice            = renderer->device();
	d3d11cfg.D3D11.pDeviceContext     = renderer->context();
	//renderer->mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))
	d3d11cfg.D3D11.pBackBufferRT      = renderer->mRenderTarget;
	d3d11cfg.D3D11.pSwapChain         = renderer->mSwapChain;

	if (!ovrHmd_ConfigureRendering(mHMD, &d3d11cfg.Config,
		                           DistortionCaps,
								   eyeFov, mEyeRenderDesc)) return;

	ovrHmd_SetEnabledCaps(mHMD, ovrHmdCap_LowPersistence |
                               ovrHmdCap_LatencyTest);

	ovrHmd_StartSensor(mHMD, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection | ovrSensorCap_Position, 0);

}

void OVRRenderer::Update(float dt)
{

}

void OVRRenderer::Render(D3DRenderer* renderer)
{

}