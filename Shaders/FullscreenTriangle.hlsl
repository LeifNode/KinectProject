#if !defined(FULLSCREEN_TRIANGLE_HLSL)
#define FULLSCREEN_TRIANGLE_HLSL

void GetTriangleVertexPosition(uint vertexId, out float4 posOut, out float2 texOut)
{
	posOut.x = (float)(id / 2) * 4.0 - 1.0;
	posOut.y = (float)(id % 2) * 4.0 - 1.0;
	posOut.z = 0.0;
	posOut.z = 1.0;

	texOut.x = (float)(id / 2) * 2.0;
	texOut.y = 1.0 - (float)(id % 2) * 2.0;
}

#endif //!defined(FULLSCREEN_TRIANGLE_HLSL)