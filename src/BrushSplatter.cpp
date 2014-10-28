#include "BrushSplatter.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "FlowField.h"

BrushSplatter::BrushSplatter()
	:mpIntermediateSurfaceTexture(NULL),
	mpIntermediateSurfaceSRV(NULL),
	mpIntermediateSurfaceRTV(NULL)
{
}

BrushSplatter::~BrushSplatter()
{
	ReleaseCOM(mpIntermediateSurfaceRTV);
	ReleaseCOM(mpIntermediateSurfaceSRV);
	ReleaseCOM(mpIntermediateSurfaceTexture);
}

void BrushSplatter::Initialize()
{
	initializeRenderTarget();

	initializeShaders();
}

void BrushSplatter::initializeRenderTarget()
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
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mpIntermediateSurfaceTexture));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(renderer->device()->CreateShaderResourceView(mpIntermediateSurfaceTexture, &srvDesc, &mpIntermediateSurfaceSRV));

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(renderer->device()->CreateRenderTargetView(mpIntermediateSurfaceTexture, &rtvDesc, &mpIntermediateSurfaceRTV));
}

void BrushSplatter::initializeShaders()
{
	Shader* testShader = gpApplication->getRenderer()->getShader("Shaders/testSplat.hlsl");

	if (testShader == NULL)
	{
		ShaderInfo shaderInfo[] = {
			{ SHADER_TYPE_VERTEX, "VS" },
			{ SHADER_TYPE_PIXEL, "PS" },
			{ SHADER_TYPE_NONE, NULL }
		};

		testShader = gpApplication->getRenderer()->loadShader(L"Shaders/testSplat.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, NULL, 0);
	}

	if (testShader)
		mBrushShaders["testSplat"] = testShader;
}

void BrushSplatter::Update(float dt)
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	renderer->setShader(mBrushShaders["testSplat"]);
	renderer->unbindTextureResources();
	renderer->context()->OMSetRenderTargets(1, &mpIntermediateSurfaceRTV, NULL);

	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	renderer->context()->ClearRenderTargetView(mpIntermediateSurfaceRTV, color);

	renderer->resetSamplerState();
	renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
	renderer->setBlendState(false);
	renderer->setViewport(FLOW_FIELD_DEFAULT_RESOLUTION, FLOW_FIELD_DEFAULT_RESOLUTION, 0, 0);

	const CBPerFrame* perFrame = renderer->getPerFrameBuffer();
	CBPerObject perObject;
	perObject.World = XMMatrixOrthographicRH(10.0f, 10.0f, 0.0f, 1.0f);
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame->ViewProj;

	renderer->bindPerFrameBuffer();
	renderer->setPerObjectBuffer(perObject);

	renderer->context()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);

	renderer->context()->Draw(4, 0);

	renderer->resetRenderTarget();
	renderer->setViewport(gpApplication->getClientWidth(), gpApplication->getClientHeight(), 0, 0);
}

