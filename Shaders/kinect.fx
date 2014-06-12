#include "ConstantBuffers.fx"

Texture2D<int> textureDepth    : register( t0 );
Texture2D<float4> textureColor : register( t1 );

SamplerState mainSampler : register( s0 );

cbuffer cbKinect : register(b2)
{
    float4  XYScale;
};

static const int DepthWidth = 640;
static const int DepthHeight = 480;
static const float2 DepthWidthHeight = float2(DepthWidth, DepthHeight);
static const float2 DepthHalfWidthHeight = DepthWidthHeight / 2.0;
static const float2 DepthHalfWidthHeightOffset = DepthHalfWidthHeight - 0.5;
static const float2 ColorWidthHeight = float2(640, 480);

static const float4 quadOffsets[4] = 
{
    float4( 0.5, -0.5, 0, 0),
	float4(-0.5, -0.5, 0, 0),
    float4( 0.5,  0.5, 0, 0),
	float4(-0.5,  0.5, 0, 0)
};

// texture lookup offsets for sampling current and nearby depth pixels
static const float2 texOffsets4Samples[4] =
{
    float2(1, 0),
    float2(0, 0),
    float2(1, 1),
	float2(0, 1),
};

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct GS_INPUT
{
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
GS_INPUT VS( )
{
    GS_INPUT output = (GS_INPUT)0;
 
    return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
// 
// Takes in a single vertex point.  Expands it into the 4 vertices of a quad.
// Depth is sampled from a texture passed in of the Kinect's depth output.
// Color is sampled from a texture passed in of the Kinect's color output mapped to depth space.
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void GS(point GS_INPUT particles[1], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
    PS_INPUT output;

    // use the minimum of near mode and standard
    static const int minDepth = 300 << 3;

    // use the maximum of near mode and standard
    static const int maxDepth = 4000 << 3;

    // texture load location for the pixel we're on 
    int3 baseLookupCoords = int3(primID % DepthWidth, primID / DepthWidth, 0);

    int depth = textureDepth.Load(baseLookupCoords);

    // check that depth is in the valid range
    if (depth < minDepth || depth > maxDepth)
    {
        return;
    }
    
    // remove player index information and convert to meters
    float realDepth = depth / 8000.0;
    
    // set the base world position here so we don't have to do it per vertex
    // convert x and y lookup coords to world space meters
    float4 WorldPos;
    WorldPos.xy = (baseLookupCoords.xy - DepthHalfWidthHeightOffset) * XYScale.xy * realDepth;
    WorldPos.z = realDepth;
    WorldPos.w = 1.0;

	WorldPos = mul(gWorld, WorldPos);

    // convert to camera space
    float4 ViewPos = mul(WorldPos, gView);

    // base color texture sample lookup coords, in [0,1]
    float2 colorTextureCoords = baseLookupCoords.xy/ColorWidthHeight;

    // determine how large to make the point sprite - scale it up a little to fill in holes
    // also make it larger if it is further away to prevent aliasing artifacts
    static const float4 PointSpriteScaleFactor = float4(1.0/DepthWidth, 1.0/DepthHeight, 0.0, 0.0) * 2.5;
    float4 quadOffsetScalingFactorInViewspace = PointSpriteScaleFactor * realDepth;

    [unroll]
    for (uint c = 0; c < 4; ++c)
    {
        // expand the quad in camera space so that it's always the same size irrespective of camera angle
        float4 ViewPosExpanded = ViewPos + quadOffsets[c] * quadOffsetScalingFactorInViewspace;
     
        // then project it
        output.Pos = mul(ViewPosExpanded, gProjection);

        // sample the color texture for the corner we're at
        output.Col = textureColor.SampleLevel(mainSampler, colorTextureCoords + texOffsets4Samples[c], 0);

        triStream.Append(output);
    }
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return float4(input.Col.rgb, 1.0);
}