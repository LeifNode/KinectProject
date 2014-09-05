#include "ConstantBuffers.hlsl"

cbuffer cbPerParticleSystem : register( b2 )
{
	float gFrameDeltaTime;
	float3 b2Pad1;
};

struct Particle
{
	float3 Position;
	float3 Velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(512, 1, 1)]
void CS(int3 ID : SV_DispatchThreadID)
{
	Particle particle = gParticles[ID.x];

	float3 acceleration = normalize(-particle.Position) * gFrameDeltaTime * 20.0f;//Pull particles towards 0,0,0

	particle.Velocity += acceleration;

	particle.Position += particle.Velocity * gFrameDeltaTime;

	if (length(particle.Position) > 20.0f)
	{
		particle.Position = normalize(particle.Position) * 20.0f;
	}

	gParticles[ID.x] = particle;
}