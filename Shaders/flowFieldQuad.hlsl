#include "ConstantBuffers.hlsl"
#include "DeferredRendering.hlsl"
#include "FullscreenQuad.hlsl"

Texture2D flowFieldTexture : register( t0 );

SamplerState texSampler : register( s0 );

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float Id : VERTEXID;
};

PS_INPUT VS( uint VertexID : SV_VertexID )
{
	PS_INPUT output;

	GetQuadVertex(VertexID, output.PositionH, output.TexCoord);

	output.PositionH = mul(gWorldViewProj, output.PositionH);
	output.Id = VertexID;

	return output;
}

#ifndef RENDERER_DEFERRED

float4 PS(PS_INPUT input) : SV_Target
{
	return float4(flowFieldTexture.Sample(texSampler, input.TexCoord).xy, 0.0f, 1.0f);
}

#else

PS_GBUFFER_OUT PS(PS_INPUT input)
{
	return PackGBuffer(float4(0.0, 0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 1, float3(1.0, 0.0, 0.0), 1);
}

#endif