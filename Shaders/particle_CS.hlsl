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
	float3 toVector = float3(0.0f, 0.0f, 0.0f);
	float att = 1.0f;
	float distance = 0.0f;

	for (int i = 0; i < 64; i++)
	{
		toVector = gParticleAttractors[i].WorldPosition - particle.Position;
		distance = length(toVector);

		//att = 1.0f / dot(float3(1.0f, gParticleAttractors[i].LinearAtt, gParticleAttractors[i].QuadraticAtt), float3(1.0f, distance, distance*distance));

		acceleration += (toVector / distance) * gFrameDeltaTime * gParticleAttractors[i].Force * att;
	}

	particle.Velocity += acceleration;

	particle.Velocity *= pow(0.5f, gFrameDeltaTime);

	particle.Position += particle.Velocity * gFrameDeltaTime;

	gParticles[ID.x] = particle;
}