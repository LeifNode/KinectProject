//Comtains helpers for physically based shading calculations

#ifndef SHADING_HLSL
#define SHADING_HLSL

#include "Math.hlsl"

float GGX(float3 n, float3 h, float r)
{
	float r2 = r * r;

	float nh = dot(n, h);
	nh *= nh;

	nh = nh * (r2 - 1.0) + 1.0;

	return nh * PI;
}

float G_Smith(float3 n, float3 v, float r)
{
	float k = (r + 1) * (r + 1);
	k /= 8.0;


}

#endif