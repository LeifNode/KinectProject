#if !defined(CONSTANT_BUFFERS_HLSL)
#define CONSTANT_BUFFERS_HLSL

#include "LightingHelper.hlsl"

cbuffer cbPerFrame : register( b0 )
{
	float4x4 gView;
	float4x4 gViewInv;
	float4x4 gProjection;
	float4x4 gProjectionInv;

	float4x4 gViewProj;
	float4x4 gViewProjInv;

	float3 gHeadPosition;
	float gpad;
	float3 gEyePosition;
	float gpad2;
	float3 gEyeDirection;
	float gpad3;

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

#endif //!defined(CONSTANT_BUFFERS_HLSL)