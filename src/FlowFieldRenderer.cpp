#include "FlowFieldRenderer.h"
#include "FlowField.h"
#include "Shader.h"
#include "ShaderHelper.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

FlowFieldRenderer::FlowFieldRenderer(FlowField* source)
	:mpSource(source),
	mpRenderShader(NULL)
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
		ShaderInfo shaderInfo[] = {
			{ SHADER_TYPE_VERTEX, "VS" },
			{ SHADER_TYPE_PIXEL, "PS" },
			{ SHADER_TYPE_NONE, NULL }
		};

		mpRenderShader = gpApplication->getRenderer()->loadShader(L"Shaders/flowFieldQuad.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, NULL, 0);
	}
}

void FlowFieldRenderer::Render(D3DRenderer* renderer)
{
	renderer->setShader(mpRenderShader);
	renderer->unbindTextureResources();

	ID3D11ShaderResourceView* textureResource = mpSource->getSurfaceSRV();

	renderer->context()->PSSetShaderResources(0, 1, &textureResource);
	renderer->resetSamplerState();
	renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
	renderer->setBlendState(false);

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