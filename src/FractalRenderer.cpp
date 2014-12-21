#include "FractalRenderer.h"
#include "D3DRenderer.h"
#include "d3dApp.h"

FractalRenderer::FractalRenderer()
	:mpFractalShader(NULL)
{

}

FractalRenderer::~FractalRenderer()
{

}

void FractalRenderer::Initialize()
{
	mpFractalShader = gpApplication->getRenderer()->getShader("Shaders/fractalQuad.hlsl");

	if (mpFractalShader == NULL)
	{
		Shader::ShaderInfo shaderInfo[] = {
			{ Shader::SHADER_TYPE_VERTEX, "VS" },
			{ Shader::SHADER_TYPE_PIXEL, "PS" },
			{ Shader::SHADER_TYPE_NONE, NULL }
		};

		mpFractalShader = gpApplication->getRenderer()->loadShader(L"Shaders/fractalQuad.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, NULL, 0);
	}
}

void FractalRenderer::Render(D3DRenderer* renderer)
{
	renderer->setShader(mpFractalShader);
	renderer->unbindTextureResources();

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