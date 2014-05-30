#include "LightingHelper.fx"

Texture2D textureDiffuse : register( t0 );
Texture2D textureNormal  : register( t1 );
Texture2D textureSpecular: register( t2 );

SamplerState mainSampler : register( s0 );

cbuffer cbPerObject : register( b0 )
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gTextureTransform;
	Material gMaterial;
};

cbuffer cbPerFrame : register( b1 )
{
	float4x4 View;
	float4x4 Projection;

	float3 gEyePosition;
	float pad;
	float3 gEyeDirection;
	float pad2;

	DirectionalLight gDirectionalLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 PosW	 : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent: BITANGENT;
	float2 Tex   : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosW = mul(gWorld, float4(vin.PosL, 1.0f)).xyz;

	vout.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));
	
	vout.Normal = normalize(mul((float3x3)gWorldInvTranspose, vin.Normal));
	vout.Tangent = normalize(mul((float3x3)gWorldInvTranspose, vin.Tangent));
	vout.Bitangent = normalize(mul((float3x3)gWorldInvTranspose, vin.Bitangent));

	vout.Tex = mul(gTextureTransform, float4(vin.Tex, 0.0f, 1.0f)).xy;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.Normal = normalize(pin.Normal);

	float3 toEye = normalize(gEyePosition - pin.PosW);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gDirectionalLight, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	//float4 color = textureDiffuse.Sample(mainSampler, pin.Tex) * (ambient + diffuse) + specular;
	float4 color = ambient + diffuse + specular;

	color.a = gMaterial.Diffuse.a;

	return color;
}