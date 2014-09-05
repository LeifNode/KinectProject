#include "ParticleSystem.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "MathHelper.h"

struct CBParticleSystemRender
{
	XMFLOAT4 ParticleColor;
	float BaseParticleScale;
	XMFLOAT3 b2pad1;
};

struct CBParticleSystemCompute
{
	float FrameDeltaTime;
	XMFLOAT3 b2Pad1;
};

ParticleSystem::ParticleSystem()
	:mpParticleRenderShader(NULL),
	mpParticleComputeShader(NULL),
	mpParticleBuffer(NULL),
	mpParticleSRV(NULL),
	mpParticleRenderCB(NULL),
	mpParticleComputeCB(NULL)
{

}

ParticleSystem::~ParticleSystem()
{
	ReleaseCOM(mpParticleBuffer);
	ReleaseCOM(mpParticleSRV);
	ReleaseCOM(mpParticleUAV);
	ReleaseCOM(mpParticleRenderCB);
	ReleaseCOM(mpParticleComputeCB);
}

void ParticleSystem::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();
	
	InitShaders(renderer);
	InitBuffers(renderer);
	InitStates(renderer);
}

void ParticleSystem::InitShaders(D3DRenderer* renderer)
{
	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_GEOMETRY, "GS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	mpParticleRenderShader = renderer->loadShader(L"Shaders/particle.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, NULL, 0); 

	ShaderInfo computeShaderInfo[] = {
		{ SHADER_TYPE_COMPUTE, "CS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	mpParticleComputeShader = renderer->loadShader(L"Shaders/particle_CS.hlsl", computeShaderInfo, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, NULL, 0);
}

void ParticleSystem::InitBuffers(D3DRenderer* renderer)
{
	/* Initializing particle buffer and access views */
	struct ParticleData
	{
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
	};

	std::vector<ParticleData> particles;
	particles.reserve(PARTICLE_COUNT);

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		ParticleData data;
		XMStoreFloat3(&data.Position, MathHelper::RandUnitVec3() * 15.0f * MathHelper::RandF(0.0f, 1.0f));
		XMStoreFloat3(&data.Velocity, MathHelper::RandUnitVec3() * 2.0f);
		//data.Velocity = XMFLOAT3(0.0f, 1.0f, 0.0f);

		particles.push_back(data);
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = sizeof(ParticleData) * PARTICLE_COUNT;
	bufferDesc.StructureByteStride = sizeof(ParticleData);
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &particles[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HR(renderer->device()->CreateBuffer(&bufferDesc, &initData, &mpParticleBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; //Format must be UNKNOWN for structured buffers
	srvDesc.Buffer.NumElements = PARTICLE_COUNT;

	HR(renderer->device()->CreateShaderResourceView(mpParticleBuffer, &srvDesc, &mpParticleSRV));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = PARTICLE_COUNT;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;

	HR(renderer->device()->CreateUnorderedAccessView(mpParticleBuffer, &uavDesc, &mpParticleUAV));

	/* Initializing constant buffers */
	
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBParticleSystemRender);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	renderer->device()->CreateBuffer(&bufferDesc, NULL, &mpParticleRenderCB);

	bufferDesc.ByteWidth = sizeof(CBParticleSystemCompute);

	renderer->device()->CreateBuffer(&bufferDesc, NULL, &mpParticleComputeCB);
}

void ParticleSystem::InitStates(D3DRenderer* renderer)
{

}

void ParticleSystem::Update(float dt)
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	renderer->setShader(mpParticleComputeShader);
	renderer->unbindTextureResources();
	renderer->context()->CSSetUnorderedAccessViews(0, 1, &mpParticleUAV, NULL);

	CBParticleSystemCompute computeConstants;
	computeConstants.FrameDeltaTime = dt;

	renderer->context()->UpdateSubresource(mpParticleComputeCB, 0, NULL, &computeConstants, 0, 0);
	renderer->setConstantBuffer(2, mpParticleComputeCB);

	renderer->context()->Dispatch(PARTICLE_GROUP_COUNT, 1, 1);

	ID3D11UnorderedAccessView* pNullUAV = NULL;
	renderer->context()->CSSetUnorderedAccessViews( 0, 1, &pNullUAV, NULL );
}

void ParticleSystem::Render(D3DRenderer* renderer)
{
	renderer->setShader(mpParticleRenderShader);
	renderer->unbindTextureResources();

	renderer->context()->VSSetShaderResources(0, 1, &mpParticleSRV);

	const CBPerFrame* perFrame = renderer->getPerFrameBuffer();
	CBPerObject perObject;
	perObject.World = XMMatrixIdentity();
	perObject.WorldInvTranspose = XMMatrixIdentity();
	perObject.WorldViewProj = perObject.World * perFrame->ViewProj;

	renderer->bindPerFrameBuffer();
	renderer->setPerObjectBuffer(perObject);

	CBParticleSystemRender particleConstants;
	particleConstants.ParticleColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	particleConstants.BaseParticleScale = 0.08f;

	renderer->context()->UpdateSubresource(mpParticleRenderCB, 0, NULL, &particleConstants, 0, 0);
	renderer->setConstantBuffer(2, mpParticleRenderCB);

	renderer->context()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);

	renderer->context()->Draw(PARTICLE_COUNT, 0);
}
