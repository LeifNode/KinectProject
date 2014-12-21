#include "ConstantBuffers.hlsl"
#include "DeferredRendering.hlsl"
#include "FullscreenQuad.hlsl"

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float4 PositionW : POSITIONW;
	float2 TexCoord : TEXCOORD;
	float Id : VERTEXID;
};

PS_INPUT VS( uint VertexID : SV_VertexID )
{
	PS_INPUT output;

	GetQuadVertex(VertexID, output.PositionH, output.TexCoord);

	output.PositionW = mul(gWorld, output.PositionH);
	output.PositionH = mul(gWorldViewProj, output.PositionH);
	
	output.Id = VertexID;

	return output;
}

#ifndef RENDERER_DEFERRED

float4 PS(PS_INPUT input) : SV_Target
{
	float3 toQuadVec = normalize(input.PositionW.xyz - gEyePosition);

	float3 currentSamplePosition = input.PositionW.xyz;

	float stepDistance = 0.005f;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float distanceSq = 10.0f;
	float3 tiledPosition;
	float traveledDistance = length(input.PositionW.xyz - gEyePosition);

	float3 lightDir = normalize(float3(-1.0f, 1.0f, -1.0f));

	for (int i = 0; i < 32; i++)
	{
		tiledPosition = (currentSamplePosition % 0.125f) - float3(0.0625f, 0.0625f, 0.0625f);

		distanceSq = dot(tiledPosition, tiledPosition);
		
		float distanceToSphere = saturate(sqrt(distanceSq) - 0.015f);
		stepDistance = distanceToSphere;

		traveledDistance += distanceToSphere;
		currentSamplePosition += toQuadVec * stepDistance;

		[branch]
		if (distanceToSphere <= 0.0f)
		{
			float diffuseFactor = dot(lightDir, normalize(tiledPosition));
			finalColor = float4(1.0f, 0.0f, 0.0f, 1.0f) * diffuseFactor;

			break;
		}

		[branch]
		if (traveledDistance * 0.2f >= 1.0f)
			return float4(0.7f, 0.7f, 0.7f, 1.0f);
	}

	return lerp(finalColor, float4(0.7f, 0.7f, 0.7f, 1.0f), saturate(traveledDistance * 0.2f));
}

#else

PS_GBUFFER_OUT PS(PS_INPUT input)
{
	return PackGBuffer(float4(0.0, 0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 1, float3(1.0, 0.0, 0.0), 1);
}

#endif