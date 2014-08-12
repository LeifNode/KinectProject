#include "FullscreenTriangle.hlsl"
#include "DeferredShading.hlsl"

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float2 PosClip: POSITION;
};

VS_OUTPUT VS( uint VertexID : SV_VertexID )
{
	VS_OUTPUT Out;

	GetTriangleVertex(VertexID, Out.Position, Out.Tex);
	Out.PosClip = Out.Position.xy;

	return Out;
}

float4 PS(VS_OUTPUT In) : SV_TARGET
{
	float depth = ConvertZToLinearDepth(DepthTexture.Sample(PointSampler, In.Tex));
	float3 worldPos = GetWorldPos(In.PosClip.xy, DepthTexture.Sample(PointSampler, In.Tex));

	//return float4(depth, depth, depth, 1.0);
	[flatten]
	if(depth < 15)
		return float4(worldPos, 1.0);
	else
		return float4(0.0, 0.0, 0.0, 1.0);

	//return float4(In.PosClip.x, In.PosClip.y, 0.0, 1.0);
}