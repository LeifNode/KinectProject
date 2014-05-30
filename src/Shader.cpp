#include "Shader.h"
#include "D3DRenderer.h"

Shader::Shader(const std::string& name)
	:mName(name),
	mpVertexShader(nullptr),
	mpInputLayout(nullptr),
	mpPixelShader(nullptr),
	mpGeometryShader(nullptr),
	mpComputeShader(nullptr)
{
}

Shader::~Shader()
{
	ReleaseCOM(mpInputLayout);
	ReleaseCOM(mpVertexShader);
	ReleaseCOM(mpPixelShader);
	ReleaseCOM(mpGeometryShader);
	ReleaseCOM(mpComputeShader);
}

void Shader::bind(D3DRenderer* renderer)
{
	ID3D11DeviceContext* context = renderer->context();

	context->IASetInputLayout(mpInputLayout);
	context->IASetPrimitiveTopology(mPrimitiveTopology);

	//Set any active shaders and disable ones not in use
	context->VSSetShader(mpVertexShader, NULL, 0);
	context->PSSetShader(mpPixelShader, NULL, 0);
	context->GSSetShader(mpGeometryShader, NULL, 0);
	context->CSSetShader(mpComputeShader, NULL, 0);
}
