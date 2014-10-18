#include "ConstantBuffers.hlsl"

struct ParticleAttractor
{
	float3 WorldPosition;
	float Force;
	float2 pad;
	float LinearAtt;
	float QuadraticAtt;
};

cbuffer cbPerParticleSystem : register( b2 )
{
	float gFrameDeltaTime;
	float3 pad;
	ParticleAttractor gParticleAttractors[64];
};

struct Particle
{
	float3 Position;
	float3 Velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1024, 1, 1)]
void CS(int3 ID : SV_DispatchThreadID)
{
	Particle particle = gParticles[ID.x];

	float3 acceleration = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 64; i++)
	{
		acceleration += normalize(gParticleAttractors[i].WorldPosition - particle.Position) * gFrameDeltaTime * gParticleAttractors[i].Force;
	}

	particle.Velocity += acceleration;

	particle.Velocity *= pow(0.5f, gFrameDeltaTime);

	particle.Position += particle.Velocity * gFrameDeltaTime;

	/*if (length(particle.Position - float3(0.0f, 2.0f, 0.0f)) > 4.0f)
	{
		particle.Position = normalize(particle.Position) * 4.0f + float3(0.0f, 2.0f, 0.0f);
	}*/

	gParticles[ID.x] = particle;
}