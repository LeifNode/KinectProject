#include "ConstantBuffers.hlsl"

cbuffer cbPerParticleSystem : register( b2 )
{
	float4 gParticleColor;
	float gBaseParticleScale;
	float3 b2pad1;
};

struct VS_INPUT
{
	uint vertexID : SV_VertexID;
};

struct Particle
{
	float3 Position : POSITION;
	float3 Velocity;
};

struct GS_INPUT
{
	float3 PositionW : POSITION;
	float3 Velocity : VELOCITY;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float3 Velocity : VELOCITY;
	float2 Tex : TEXCOORD;
};

StructuredBuffer<Particle> gParticleBuffer : register(u0);

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output;

	output.PositionW = mul(gWorld, float4(gParticleBuffer[input.vertexID].Position, 1.0f)).xyz;
	output.Velocity = gParticleBuffer[input.vertexID].Velocity;

	return output;
}

[maxvertexcount(4)]
void GS(point GS_INPUT input[1], uint primId : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	float3 toEye = normalize(gEyePosition - input[0].PositionW);

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, toEye);

	float2 texCoords[4] =
	{
		float2(0.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
	};

	float particleHalfSize = gBaseParticleScale * 0.5f;

	float3 offsets[4] = 
	{
		right * -particleHalfSize + up * particleHalfSize,
		right * -particleHalfSize + up * -particleHalfSize,
		right * particleHalfSize + up * particleHalfSize,
		right * particleHalfSize + up * -particleHalfSize,
	};

	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		output.PositionH = mul(gViewProj, float4(input[0].PositionW + offsets[i], 1.0f));
		output.Tex = texCoords[i];
		output.Velocity = input[0].Velocity;

		triStream.Append(output);
	}
}

float4 PS(PS_INPUT input) : SV_Target
{
	return float4(input.Tex.r, input.Tex.g, 0.0f, 1.0f);
}