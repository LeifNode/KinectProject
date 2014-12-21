#include "FlowFieldRenderer.h"
#include "FlowField.h"
#include "Shader.h"
#include "ShaderHelper.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

FlowFieldRenderer::FlowFieldRenderer(FlowField* source)
	:mpSource(source),
	mpRenderShader(NULL),
	mpInternalSource(NULL)
{

}

FlowFieldRenderer::FlowFieldRenderer(ID3D11ShaderResourceView* source)
	:mpRenderShader(NULL),
	mpInternalSource(source),
	mpSource(NULL)
{

}

FlowFieldRenderer::~FlowFieldRenderer()
{
	mpRenderShader = NULL;
	mpSource = NULL;
}

void FlowFieldRenderer::Initialize()
{
	mpRenderShader = gpApplication->getRenderer()->getShader("Shaders/flowFieldQuad.hlsl");

	if (mpRenderShader == NULL)
	{
		Shader::ShaderInfo shaderInfo[] = {
			{ Shader::SHADER_TYPE_VERTEX, "VS" },
			{ Shader::SHADER_TYPE_PIXEL, "PS" },
			{ Shader::SHADER_TYPE_NONE, NULL }
		};

		mpRenderShader = gpApplication->getRenderer()->loadShader(L"Shaders/flowFieldQuad.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, NULL, 0);
	}
}

void FlowFieldRenderer::Render(D3DRenderer* renderer)
{
	renderer->setShader(mpRenderShader);
	renderer->unbindTextureResources();

	ID3D11ShaderResourceView* textureResource;

	if (mpSource)
		textureResource = mpSource->getSurfaceSRV();
	else if (mpInternalSource)
		textureResource = mpInternalSource;

	renderer->resetSamplerState();
	renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
	renderer->setBlendState(false);
	renderer->context()->PSSetShaderResources(0, 1, &textureResource);
	

	const CBPerFrame* perFrame = renderer->getPerFrameBuffer();
	CBPerObject perObject;
	perObject.World = transform.getTransform();
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame->ViewProj;

	renderer->bindPerFrameBuffer();
	renderer->setPerObjectBuffer(perObject);

	renderer->context()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);

	renderer->context()->Draw(4, 0);
}