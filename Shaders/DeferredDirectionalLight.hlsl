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
	SURFACE_DATA surface = UnpackGBuffer(In.Tex);

	float3 worldPos = GetWorldPos(In.PosClip.xy, surface.Depth);
	float3 toEye = normalize(gEyePosition - worldPos);

	//return float4(depth, depth, depth, 1.0);
	[branch]
	if(surface.LinearDepth > 5000.0)
		return float4(0.0, 0.125, 0.3, 1.0);


	Material mat;
	mat.Ambient = float4(1.0, 1.0, 1.0, 1.0);
	mat.Diffuse = surface.Diffuse;
	mat.Specular = float4(surface.Specular, surface.SpecPow);
	
	DirectionalLight light;
	light.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	light.Ambient = float4(0.01f, 0.01f, 0.01f, 0.01f);
	light.Direction = normalize(float3(-1.0f, -1.0f, -1.0f));

	float4 ambient = float4(0.0, 0.0, 0.0, 0.0);
	float4 diffuse = float4(0.0, 0.0, 0.0, 0.0);
	float4 specular = float4(0.0, 0.0, 0.0, 0.0);

	ComputeDirectionalLight(mat, light, surface.Normal, toEye, ambient, diffuse, specular);

	return float4(diffuse + specular + surface.Emissive.xyzz);
}