#include "FlowField.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "TextureManager.h"
#include "Texture.h"

FlowField::FlowField(unsigned int resolution, unsigned int downsampling)
	:mRenderSurfaceResolution(resolution),
	mFieldResolutionDownsampling(downsampling),
	mpRenderSurfaceTexture(NULL),
	mpDensityMapTexture(NULL),
	mpFlowFieldTexture(NULL),
	mpVelocityFieldTexture(NULL),
	mpWavefrontTexture(NULL),
	mpRenderSurfaceSRV(NULL),
	mpDensityMapSRV(NULL),
	mpFlowFieldSRV(NULL),
	mpVelocityFieldSRV(NULL),
	mpWavefrontSRV(NULL),
	mpDensityMapUAV(NULL),
	mpFlowFieldUAV(NULL),
	mpVelocityFieldUAV(NULL),
	mpWavefrontUAV(NULL),
	mpRenderSurfaceRTV(NULL)
{
	mFieldResolution = resolution / downsampling;
}

FlowField::~FlowField()
{
	cleanup();
}

void FlowField::cleanup()
{
	ReleaseCOM(mpRenderSurfaceTexture);
	ReleaseCOM(mpDensityMapTexture);
	ReleaseCOM(mpFlowFieldTexture);
	ReleaseCOM(mpVelocityFieldTexture);
	ReleaseCOM(mpWavefrontTexture);

	ReleaseCOM(mpRenderSurfaceSRV);
	ReleaseCOM(mpDensityMapSRV);
	ReleaseCOM(mpFlowFieldSRV);
	ReleaseCOM(mpVelocityFieldSRV);
	ReleaseCOM(mpWavefrontSRV);

	ReleaseCOM(mpDensityMapUAV);
	ReleaseCOM(mpFlowFieldUAV);
	ReleaseCOM(mpVelocityFieldUAV);
	ReleaseCOM(mpWavefrontUAV);

	ReleaseCOM(mpRenderSurfaceRTV);
}

void FlowField::Initialize()
{
	mpParticleTexture = gpApplication->getTextureManager()->getTexture("particle.dds");

	initializeRenderTarget();
	initializeFields();
}

void FlowField::unbindSRVs()
{

}

void FlowField::unbindUAVs()
{

}

void FlowField::unbindRTV()
{

}

void FlowField::initializeRenderTarget()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	texDesc.Width = mRenderSurfaceResolution;
	texDesc.Height = mRenderSurfaceResolution;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpRenderSurfaceTexture));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(renderer->device()->CreateShaderResourceView(mpRenderSurfaceTexture, &srvDesc, &mpRenderSurfaceSRV));

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(renderer->device()->CreateRenderTargetView(mpRenderSurfaceTexture, &rtvDesc, &mpRenderSurfaceRTV));
}

void FlowField::initializeFields()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	//Density field
	texDesc.Width = mFieldResolution;
	texDesc.Height = mFieldResolution;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R32_UINT;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpDensityMapTexture));

	srvDesc.Format = DXGI_FORMAT_R32_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(renderer->device()->CreateShaderResourceView(mpDensityMapTexture, &srvDesc, &mpDensityMapSRV));

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	HR(renderer->device()->CreateUnorderedAccessView(mpDensityMapTexture, &uavDesc, &mpDensityMapUAV));

	//Flow field
	texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpFlowFieldTexture));

	srvDesc.Format = DXGI_FORMAT_R8G8_UNORM;
	HR(renderer->device()->CreateShaderResourceView(mpFlowFieldTexture, &srvDesc, &mpFlowFieldSRV));

	HR(renderer->device()->CreateUnorderedAccessView(mpFlowFieldTexture, &uavDesc, &mpFlowFieldUAV));

	//Velocity field
	texDesc.Format = DXGI_FORMAT_R16G16_UNORM;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpVelocityFieldTexture));

	srvDesc.Format = DXGI_FORMAT_R16G16_UNORM;
	HR(renderer->device()->CreateShaderResourceView(mpVelocityFieldTexture, &srvDesc, &mpVelocityFieldSRV));
	
	HR(renderer->device()->CreateUnorderedAccessView(mpVelocityFieldTexture, &uavDesc, &mpVelocityFieldUAV));

	//Wavefront field
	texDesc.Format = DXGI_FORMAT_R32_UINT;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpWavefrontTexture));

	srvDesc.Format = DXGI_FORMAT_R32_UINT;
	HR(renderer->device()->CreateShaderResourceView(mpWavefrontTexture, &srvDesc, &mpWavefrontSRV));

	HR(renderer->device()->CreateUnorderedAccessView(mpWavefrontTexture, &uavDesc, &mpWavefrontUAV));
}