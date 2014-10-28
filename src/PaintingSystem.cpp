#include "PaintingSystem.h"
#include "FlowField.h"
#include "FlowFieldRenderer.h"
#include "BrushSplatter.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

PaintingSystem::PaintingSystem()
	:mpFlowField(NULL),
	mpFlowFieldRenderer(NULL),
	mpBrushSplatter(NULL),
	mpCompositedSurfaceTexture(NULL),
	mpCompositedSurfaceSRV(NULL),
	mpCompositedSurfaceRTV(NULL)
{

}

PaintingSystem::~PaintingSystem()
{
	SAFE_DELETE(mpBrushSplatter);
	SAFE_DELETE(mpFlowFieldRenderer);
	SAFE_DELETE(mpFlowField);

	ReleaseCOM(mpCompositedSurfaceSRV);
	ReleaseCOM(mpCompositedSurfaceRTV);
	ReleaseCOM(mpCompositedSurfaceTexture);
}

void PaintingSystem::Initialize()
{
	mpFlowField = new FlowField();
	mpBrushSplatter = new BrushSplatter();

	mpFlowField->Initialize();
	mpBrushSplatter->Initialize();

	mpFlowFieldRenderer = new FlowFieldRenderer(mpBrushSplatter->getIntermediateSurfaceSRV());
	mpFlowFieldRenderer->Initialize();

	initializeCompositeSurface();
}

void PaintingSystem::initializeCompositeSurface()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	texDesc.Width = FLOW_FIELD_DEFAULT_RESOLUTION;
	texDesc.Height = FLOW_FIELD_DEFAULT_RESOLUTION;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpCompositedSurfaceTexture));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(renderer->device()->CreateShaderResourceView(mpCompositedSurfaceTexture, &srvDesc, &mpCompositedSurfaceSRV));

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(renderer->device()->CreateRenderTargetView(mpCompositedSurfaceTexture, &rtvDesc, &mpCompositedSurfaceRTV));
}

void PaintingSystem::Update(float dt)
{
	mpBrushSplatter->Update(dt);

	D3DRenderer* renderer = gpApplication->getRenderer();

	//renderer->context()->CopyResource(mpFlowField->getSurfaceResource(), mpBrushSplatter->getIntermediateSurfaceResource());
}

void PaintingSystem::Render(D3DRenderer* renderer)
{
	mpFlowFieldRenderer->Render(renderer);
}