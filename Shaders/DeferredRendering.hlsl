//Handles rendering to the GBuffer

#if !defined(DEFERRED_RENDERING_HLSL)
#define DEFERRED_RENDERING_HLSL

#ifndef SPECULAR_POW_RANGE_MIN
#define SPECULAR_POW_RANGE_MIN 1.0
#endif

#ifndef SPECULAR_POW_RANGE_MAX
#define SPECULAR_POW_RANGE_MAX 240.0
#endif

#ifndef EMISSIVE_RANGE_MIN //(1 / 16)
#define EMISSIVE_RANGE_MIN 0.0625
#endif

#ifndef EMISSIVE_RANGE_MAX
#define EMISSIVE_RANGE_MAX 16.0
#endif

#include "ConstantBuffers.hlsl"

struct PS_GBUFFER_OUT
{
	float4 Diffuse : SV_TARGET0;
	float3 Normal : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Emissive : SV_TARGET3;
};

PS_GBUFFER_OUT PackGBuffer(float3 DiffuseColor, float3 Normal, float3 SpecularColor, float SpecPower, float3 EmissiveColor, float EmissiveMultiplier)
{
	PS_GBUFFER_OUT Out;

	// Normalize the specular power and emissive multiplier
	float SpecPowerNorm = max(0.0001, (SpecPower - SPECULAR_POW_RANGE_MIN) / SPECULAR_POW_RANGE_MAX);
	float EmissiveMultiplier = max(0.0001, (EmissiveMultiplier - EMISSIVE_RANGE_MIN) / EMISSIVE_RANGE_MAX);

	// Pack all the data into the GBuffer structure
	Out.Diffuse = float4(BaseColor.rgb, 0.0);
	Out.Normal = Normal * 0.5 + 0.5
	Out.Specular = float4(SpecularColor, SpecPowerNorm);
	Out.Emissive = float4(EmissiveColor, EmissiveMultiplier);

	return Out;
}

#endif //!defined(DEFERRED_RENDERING_HLSL)