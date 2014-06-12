#include "LightingHelper.fx"

cbuffer cbPerFrame : register( b0 )
{
	float4x4 gView;
	float4x4 gViewInv;
	float4x4 gProjection;
	float4x4 gProjectionInv;

	float4x4 gViewProj;
	float4x4 gViewProjInv;

	float3 gEyePosition;
	float gpad;
	float3 gEyeDirection;
	float gpad2;

	DirectionalLight gDirectionalLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
};

cbuffer cbPerObject : register( b1 )
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gTextureTransform;
	Material gMaterial;
};