#include "ConstantBuffers.hlsl"
#include "DeferredRendering.hlsl"

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

		float nextPointLength = dot(toNextPoint, toNextPoint);

		
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

	/*int i = 0;
	float halfLineThickness = gLineThickness * 0.5f;

	const float4 screenPoints[2] =
	{
		mul(gViewProj, float4(points[1], 1.0f)),
		mul(gViewProj, float4(points[2], 1.0f)),
	};


	float2 toPoint = normalize(screenPoints[0].xy - screenPoints[1].xy);
	float4 offsetVec = float4(normalize(float2(-toPoint.y, toPoint.x)), 0.0f, 0.0f);

	const float4 finalPoints[4] =
	{
		screenPoints[0] + offsetVec * halfLineThickness * screenPoints[0].w,
		screenPoints[0] - offsetVec * halfLineThickness * screenPoints[0].w,
		screenPoints[1] + offsetVec * halfLineThickness * screenPoints[1].w,
		screenPoints[1] - offsetVec * halfLineThickness * screenPoints[1].w,
	};

	[unroll]
	for (i = 0; i < 4; i++)
	{
		output.PositionH = finalPoints[i];
		output.Id = primID;

		triStream.Append(output);
	}*/
}

#ifndef RENDERER_DEFERRED

//TODO: Trick depth buffer into thinking this is a 3d object? SV_Depth
float4 PS(PS_INPUT input) : SV_Target
{
	//float4 color = gLineColor + float4(0.0f, frac((float)input.Id * 0.0001f), frac((float)input.Id * 0.0003f), 0.0f);

	float4 color = gLineColor + float4(0.0f, frac(input.Id * 0.001f), frac(input.Id * 0.0003f), 0.0f);
	//float4 color = float4(input.PositionH.yyy / 720.0f, 1.0f);
	color.a = 1.0;

	return color;
}

#else

PS_GBUFFER_OUT PS(PS_INPUT input)
{
	//float4 color = gLineColor + float4(0.0f, frac((float)input.Id * 0.0001f), frac((float)input.Id * 0.0003f), 0.0f);

	float4 color = gLineColor + float4(0.0f, frac(input.Id * 0.001f), frac(input.Id * 0.0003f), 0.0f);
	//float4 color = float4(input.PositionH.yyy / 720.0f, 1.0f);
	color.a = 1.0;

	return PackGBuffer(float4(0.0, 0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 1, color.rgb, 1);
}

#endif