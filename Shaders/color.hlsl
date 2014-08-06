#include "ConstantBuffers.hlsl"

Texture2D textureDiffuse : register( t0 );
Texture2D textureNormal  : register( t1 );
Texture2D textureSpecular: register( t2 );

SamplerState mainSampler : register( s0 );

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

	float4 ambient = float4(0.05f, 0.05f, 0.05f, 1.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//diffuse *= dot(pin.Normal, normalize(float3(0.5f, 1.0f, 1.0f)));

	float4 A, D, S;

	DirectionalLight light;
	light.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	light.Ambient = float4(0.01f, 0.01f, 0.01f, 0.01f);
	light.Direction = normalize(float3(-1.0f, -1.0f, -1.0f));

	Material mat;
	mat.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.Specular = float4(1.0f, 1.0f, 1.0f, 80.0f);
	mat.Ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);

	ComputeDirectionalLight(mat, light, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	/*ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;*/

	//float4 color = textureDiffuse.Sample(mainSampler, pin.Tex) * (ambient + diffuse) + specular;

	float4 color = ambient + diffuse + specular;

	color.a = gMaterial.Diffuse.a;

	return color;
}