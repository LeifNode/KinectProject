#include "ConstantBuffers.fx"

Texture2D textAtlus : register( t0 );

SamplerState textSampler : register( s0 );

struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 TexTL : TEXCOORD0;
	float2 TexBR : TEXCOORD1;
	float2 Dimensions : DIMENSION;
};

struct GS_INPUT
{
	float3 Pos : POSITION;
	float2 TexTL : TEXCOORD0;
	float2 TexBR : TEXCOORD1;
	float2 Dimensions : DIMENSION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexUV : TEXCOORD;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output = (GS_INPUT)0;

	output.Pos = input.Pos;
	output.TexTL = input.TexTL;
	output.TexBR = input.TexBR;
	output.Dimensions = input.Dimensions;

	return output;
}

[maxvertexcount(4)]
void GS(point GS_INPUT input[1], uint primId : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	float2 texCoords[4] =
	{
		float2(input[0].TexBR.x, input[0].TexTL.y),
		float2(input[0].TexBR.x, input[0].TexBR.y),
		float2(input[0].TexTL.x, input[0].TexTL.y),
		float2(input[0].TexTL.x, input[0].TexBR.y)
	};

	float2 halfDimensions = input[0].Dimensions * 0.5f;

	float3 offsets[4] = 
	{
		float3(-halfDimensions.x, halfDimensions.y, 0.0f),
		float3(-halfDimensions.x, -halfDimensions.y, 0.0f),
		float3(halfDimensions.x, halfDimensions.y, 0.0f),
		float3(halfDimensions.x, -halfDimensions.y, 0.0f),
	};

	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		output.Pos = mul(gWorldViewProj, float4(input[0].Pos + offsets[i], 1.0f));
		output.TexUV = texCoords[i];

		triStream.Append(output);
	}
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = float4(textAtlus.Sample(textSampler, input.TexUV).r, 0.0f, 0.0f, 1.0f);

	clip(color.r < 0.1f ? -1:0);//Handle transparency

	return color;
}