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
	float Id : VERTEXID;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output;

	output.PositionW = mul(gWorld, float4(input.Position, 1.0f)).xyz;

	return output;
}

[maxvertexcount(4)]
void GS(lineadj GS_INPUT points[4], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;
	int i = 0;

	float3 midPoint;
	float3 toEye;
	float3 toNextPoint;
	float halfLineThickness = gLineThickness * 0.5f;

	float3 upAxis;
	float3 neighborUpAxis[3];

	midPoint = (points[1].PositionW + points[2].PositionW) * 0.5f;
	toEye = normalize(midPoint - gEyePosition);
	toNextPoint = normalize(points[1].PositionW - points[2].PositionW);
	upAxis = normalize(cross(toEye, toNextPoint));

	[unroll]
	for (i = 0; i < 3; i += 2)
	{
		midPoint = (points[i].PositionW + points[i + 1].PositionW) * 0.5f;
		toEye = normalize(midPoint - gEyePosition);
		toNextPoint = points[i].PositionW - points[i + 1].PositionW;

		float nextPointLength = toNextPoint.x * toNextPoint.x + 
						        toNextPoint.y * toNextPoint.y + 
						        toNextPoint.z * toNextPoint.z;

		
		[flatten] 
		if (nextPointLength == 0.0f)
			neighborUpAxis[i] = upAxis;
		else
			neighborUpAxis[i] = normalize(cross(toEye, normalize(toNextPoint)));
	}

	const float3 neighborPoints[4] =
	{
		points[1].PositionW + neighborUpAxis[0] * halfLineThickness,
		points[1].PositionW - neighborUpAxis[0] * halfLineThickness,
		points[2].PositionW + neighborUpAxis[2] * halfLineThickness,
		points[2].PositionW - neighborUpAxis[2] * halfLineThickness,
	};

	const float3 outPoints[4] = 
	{
		(points[1].PositionW + upAxis * halfLineThickness + neighborPoints[0]) * 0.5f,
		(points[1].PositionW - upAxis * halfLineThickness + neighborPoints[1]) * 0.5f,
		(points[2].PositionW + upAxis * halfLineThickness + neighborPoints[2]) * 0.5f,
		(points[2].PositionW - upAxis * halfLineThickness + neighborPoints[3]) * 0.5f,
	};

	[unroll]
	for (i = 0; i < 4; i++)
	{
		output.PositionH = mul(gViewProj, float4(outPoints[i], 1.0f));
		output.Id = primID;

		triStream.Append(output);
	}
}

//TODO: Trick depth buffer into thinking this is a 3d object? SV_Depth
float4 PS(PS_INPUT input) : SV_Target
{
	//float4 color = gLineColor + float4(0.0f, frac((float)input.Id * 0.0001f), frac((float)input.Id * 0.0003f), 0.0f);

	float4 color = gLineColor + float4(0.0f, frac(input.Id * 0.001f), frac(input.Id * 0.0003f), 0.0f);

	return color;
}