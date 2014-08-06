#if !defined(DEFERRED_SHADING_HLSL)
#define DEFERRED_SHADING_HLSL

Texture2D<float4> DiffuseTexture  : register( t0 );
Texture2D<float3> NormalTexture   : register( t1 );
Texture2D<float4> SpecularTexture : register( t2 );
Texture2D<float4> EmissiveTexture : register( t4 );
Texture2D<float> DepthTexture     : register( t5 );

SamplerState PointSampler         : register( s0 );



#endif //!defined(DEFERRED_SHADING_HLSL)