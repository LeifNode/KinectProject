#include "ConstantBuffers.fx"

cbuffer cbPerLine : register( b2 )
{
	uint gPointCount;
	uint3 pad;
	float4 gLineColor;
	float gLineThickness;
	float3 pad2;
};

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct GS_INPUT
{
	float3 PositionW : POSITION;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output;

	output.PositionW = mul(gWorld, float4(input.Position, 1.0f)).xyz;

	return output;
}

[maxvertexcount(4)]
void GS(line GS_INPUT points[2], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	float3 midPoint = (points[0].PositionW + points[1].PositionW) / 2.0f;
	float3 toEye = normalize(midPoint - gEyePosition);
	float3 toNextPoint = normalize(points[0].PositionW - points[1].PositionW);
	float3 upAxis = normalize(cross(toEye, toNextPoint));
	float halfLineThickness = gLineThickness * 0.5f;

	const float3 outPoints[4] = 
	{
		points[0].PositionW - upAxis * halfLineThickness,
		points[0].PositionW + upAxis * halfLineThickness,
		points[1].PositionW - upAxis * halfLineThickness,
		points[1].PositionW + upAxis * halfLineThickness,
	};

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		output.PositionH = mul(gViewProj, float4(outPoints[i], 1.0f));

		triStream.Append(output);
	}
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = gLineColor;

	return color;
}