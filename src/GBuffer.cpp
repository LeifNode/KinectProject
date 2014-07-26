#include "GBuffer.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

GBuffer::GBuffer()
	:mpDiffuseTexture(NULL),
	mpDepthStencilTexture(NULL),
	mpNormalTexture(NULL),
	mpSpecularTexture(NULL),
	mpEmissiveTexture(NULL),
	mpDiffuseTargetView(NULL),
	mpDepthStencilTargetView(NULL),
	mpNormalTargetView(NULL),
	mpSpecularTargetView(NULL),
	mpEmissiveTargetView(NULL),
	mpDepthStencilState(NULL)
{

}

GBuffer::~GBuffer()
{
	
}

void GBuffer::Initialize(int width, int height)
{
	//OnResize(width, height);

	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = TRUE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	desc.FrontFace = stencilMarkOp;
	desc.BackFace = stencilMarkOp;

	gpApplication->getRenderer()->device()->CreateDepthStencilState(&desc, &mpDepthStencilState);
}

void GBuffer::DeInit()
{
	ReleaseCOM(mpDiffuseTargetView);
	ReleaseCOM(mpDepthStencilTargetView);
	ReleaseCOM(mpNormalTargetView);
	ReleaseCOM(mpSpecularTargetView);
	ReleaseCOM(mpEmissiveTargetView);
	ReleaseCOM(mpDepthStencilState);

	SAFE_DELETE(mpDiffuseTexture);
	SAFE_DELETE(mpDepthStencilTexture);
	SAFE_DELETE(mpNormalTexture);
	SAFE_DELETE(mpSpecularTexture);
	SAFE_DELETE(mpEmissiveTexture);
}

void GBuffer::OnResize(int width, int height)
{
	ReleaseCOM(mpDiffuseTargetView);
	ReleaseCOM(mpDepthStencilTargetView);
	ReleaseCOM(mpNormalTargetView);
	ReleaseCOM(mpSpecularTargetView);
	ReleaseCOM(mpEmissiveTargetView);

	SAFE_DELETE(mpDiffuseTexture);
	SAFE_DELETE(mpDepthStencilTexture);
	SAFE_DELETE(mpNormalTexture);
	SAFE_DELETE(mpSpecularTexture);
	SAFE_DELETE(mpEmissiveTexture);

	D3DRenderer* renderer = gpApplication->getRenderer();

	static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
	static const DXGI_FORMAT diffuseTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalTextureFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	static const DXGI_FORMAT specularTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT emissiveTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	static const DXGI_FORMAT depthStencilRenderViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const DXGI_FORMAT diffuseRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalRenderViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	static const DXGI_FORMAT specularRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT emissiveRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Initialize textures
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	desc.Format = depthStencilTextureFormat;
	mpDepthStencilTexture = renderer->createTexture(&desc, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Format = diffuseTextureFormat;
	mpDiffuseTexture = renderer->createTexture(&desc);

	desc.Format = normalTextureFormat;
	mpNormalTexture = renderer->createTexture(&desc);

	desc.Format = specularTextureFormat;
	mpSpecularTexture = renderer->createTexture(&desc);

	desc.Format = emissiveTextureFormat;
	mpEmissiveTexture = renderer->createTexture(&desc);

	//Create resource views
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = 
	{
		depthStencilRenderViewFormat,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};

	renderer->device()->CreateDepthStencilView(mpDepthStencilTexture->getD3DTexture(), &dsvd, &mpDepthStencilTargetView);

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd = 
	{
		diffuseRenderViewFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	renderer->device()->CreateRenderTargetView(mpDiffuseTexture->getD3DTexture(), &rtsvd, &mpDiffuseTargetView);

	rtsvd.Format = normalRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpNormalTexture->getD3DTexture(), &rtsvd, &mpNormalTargetView);

	rtsvd.Format = specularRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpSpecularTexture->getD3DTexture(), &rtsvd, &mpSpecularTargetView);

	rtsvd.Format = emissiveRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpEmissiveTexture->getD3DTexture(), &rtsvd, &mpEmissiveTargetView);
}

void GBuffer::bindRenderTargets()
{

}