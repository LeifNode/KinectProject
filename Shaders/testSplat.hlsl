#include "ConstantBuffers.hlsl"
#include "FullscreenQuad.hlsl"

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

PS_INPUT VS( uint VertexID : SV_VertexID )
{
	PS_INPUT output;

	GetQuadVertex(VertexID, output.PositionH, output.TexCoord);

	//output.PositionH = mul(gWorld, output.PositionH);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float posLength = length((input.TexCoord));

	return float4(posLength, 0.0f, 0.0f, 1.0f);
}