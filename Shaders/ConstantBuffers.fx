#include "LightingHelper.fx"

cbuffer cbPerFrame : register( b0 )
{
	float4x4 View;
	float4x4 ViewInv;
	float4x4 Projection;
	float4x4 ProjectionInv;

	float4x4 ViewProj;
	float4x4 ViewProjInv;

	float3 gEyePosition;
	float pad;
	float3 gEyeDirection;
	float pad2;

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