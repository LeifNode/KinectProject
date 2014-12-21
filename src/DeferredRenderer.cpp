#include "DeferredRenderer.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "GBuffer.h"

DeferredRenderer::DeferredRenderer()
	:mpDirectionalLightPass(NULL)
{

}

DeferredRenderer::~DeferredRenderer()
{
	mpDirectionalLightPass = NULL;
}

void DeferredRenderer::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	Shader::ShaderInfo shaderInfo[] = {
		{ Shader::SHADER_TYPE_VERTEX, "VS" },
		{ Shader::SHADER_TYPE_PIXEL, "PS" },
		{ Shader::SHADER_TYPE_NONE, NULL }
	};

	mpDirectionalLightPass = renderer->loadShader(L"Shaders/DeferredDirectionalLight.hlsl", shaderInfo, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, NULL, 0);
}

void DeferredRenderer::OnResize()
{

}

void DeferredRenderer::Render(D3DRenderer* renderer)
{
	renderDirectionalLight(renderer);
}

void DeferredRenderer::renderDirectionalLight(D3DRenderer* renderer)
{
	renderer->setShader(mpDirectionalLightPass);
	renderer->getGBuffer()->bindTextures();
	renderer->getGBuffer()->bindSampler();
	renderer->bindPerFrameBuffer();

	ID3D11DeviceContext* context = renderer->context();

	context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	context->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);

	context->Draw(3, 0);
}