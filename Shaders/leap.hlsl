#include "ConstantBuffers.hlsl"

Texture2D textureDiffuse : register( t0 );
Texture2D offsetTexture : register( t1 );

SamplerState mainSampler : register( s0 );

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float3 PositionV : POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer CBLeap : register( b2 )
{
	float2 gRayScale;
	float2 gRayOffset;
	float gHandDistance;
	float gOffsetX;
	float2 b2Pad1;
};

static const float3 Positions[4] =
{
	float3(-1.0f, 1.0f, 0.0f),
	float3(-1.0f, -1.0f, 0.0f),
	float3(1.0f, 1.0f, 0.0f),
	float3(1.0f, -1.0f, 0.0f),
};

static const float2 TexCoords[4] =
{
	float2(0.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f),
};

static const float4 SkinColor = float4(0.847f, 0.643f, 0.588f, 1.0f);

PS_INPUT VS( uint VertexID : SV_VertexID )
{
	PS_INPUT output;

	output.PositionV = Positions[VertexID] * 4.0f;
	output.PositionV.z = -gHandDistance - 0.06f;
	//output.PositionV.z = -1.0f;
	output.PositionV.xy *= gHandDistance;
	//output.PositionV.z -= 0.06f;
	output.PositionV.x += gOffsetX;
	output.PositionV.y += 0.05f; //Compensation for messed up rift rendering on this program
	output.Tex = TexCoords[VertexID];
	output.PositionH = mul(gProjection, float4(output.PositionV, 1.0f));

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 brightness = textureDiffuse.Sample(mainSampler, input.Tex);
	float4 distortion = offsetTexture.Sample(mainSampler, input.Tex);

	float4 indexIntoRawData = offsetTexture.Sample(mainSampler, input.Tex);

	float2 offsetVector =  input.Tex - indexIntoRawData.xy;
	float2 samplePosition = input.Tex - offsetVector;

	float4 finalColor;

    finalColor = textureDiffuse.Sample(mainSampler, float2(indexIntoRawData.x, -indexIntoRawData.y));

	clip(finalColor.r < 0.2f ? -1:1);

	return float4(finalColor.rrr * SkinColor.rgb * 1.15f, 1.0f);
	//return float4(offsetVector.r * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f);
}

//
//float4 PS(PS_INPUT input) : SV_Target
//{
//	// Unwarp the point. Correct oculus distortion, if applicable
//	float2 ray = input.uv * float2(8.0, 8.0) - float2(4.0, 4.0); // range [-4, 4] x [-4, 4]
//
//	float2 texDist = ray * float2(_RayScaleX, _RayScaleY) + float2(_RayOffsetX, _RayOffsetY);
//
//	float texImageX = DecodeFloatRGBA(tex2D(_DistortX, texDist));
//
//	texImageX = texImageX * 2.3 - 0.6; // [0..1) -> [-0.6..1.7)
//	float texImageY = DecodeFloatRGBA(tex2D(_DistortY, texDist));
//
//	texImageY = texImageY * 2.3 - 0.6;
//	float2 texCoord = float2(texImageX, texImageY);
//	// Look up point in our texture
//	if (texCoord.x > 0.0 && texCoord.x < 1.0 && texCoord.y > 0.0 && texCoord.y < 1.0) {
//	texCoord.y = 1.0 - texCoord.y; // why does the distortion map flip us upside-down?
//	float a = tex2D(_MainTex, texCoord).a;
//	// for debug: draw the normalized distortion map in blue, hand in red
//	return float4(a, a, a, 1.0);
//	}
//	return float4(1.0, 0.0, 0.0, 1.0);
//}