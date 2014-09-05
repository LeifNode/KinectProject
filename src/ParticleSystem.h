#pragma once

#include "d3dStd.h"
#include "Shader.h"

class D3DRenderer;

class ParticleSystem
{
	static const int PARTICLE_GROUP_COUNT = 10000;
	static const int PARTICLE_GROUP_SIZE = 512;
	static const int PARTICLE_COUNT = PARTICLE_GROUP_COUNT * PARTICLE_GROUP_SIZE;

public:
	ParticleSystem();
	~ParticleSystem();

	void Initialize();

	void Update(float dt);

	void Render(D3DRenderer* renderer);

private:
	void InitBuffers(D3DRenderer* renderer);
	void InitShaders(D3DRenderer* renderer);
	void InitStates(D3DRenderer* renderer);

private:
	Shader* mpParticleRenderShader;
	Shader* mpParticleComputeShader;

	ID3D11Buffer* mpParticleBuffer;
	ID3D11ShaderResourceView* mpParticleSRV;
	ID3D11UnorderedAccessView* mpParticleUAV;

	ID3D11Buffer* mpParticleRenderCB;
	ID3D11Buffer* mpParticleComputeCB;
};