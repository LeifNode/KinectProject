#include "d3dStd.h"

class D3DRenderer;

//Handles all textures used by the flow field

class FlowField
{
public:
	FlowField(unsigned int resolution = 1024, unsigned int downsampling = 8);
	~FlowField();

	void Initialize();

	void unbindSRVs();
	void unbindUAVs();
	void unbindRTV();

	ID3D11ShaderResourceView* getDensitySRV() const { return mpDensityMapSRV; };
	ID3D11ShaderResourceView* getFlowFieldSRV() const { return mpFlowFieldSRV; };
	ID3D11ShaderResourceView* getVelocitySRV() const { return mpVelocityFieldSRV; };
	ID3D11ShaderResourceView* getWavefrontSRV() const { return mpWavefrontSRV; };

	ID3D11UnorderedAccessView* getDensityUAV() const { return mpDensityMapUAV; }
	ID3D11UnorderedAccessView* getFlowFieldUAV() const { return mpFlowFieldUAV; }
	ID3D11UnorderedAccessView* getVelocityUAV() const { return mpVelocityFieldUAV; }
	ID3D11UnorderedAccessView* getWavefrontUAV() const { return mpWavefrontUAV; }

	ID3D11RenderTargetView* getSurfaceRTV() const { return mpRenderSurfaceRTV; }

private:
	void cleanup();

	void initializeRenderTarget();
	void initializeFields();


private:
	//Particle texture loaded from particle.dds. Just a circle with a gradient falloff
	class Texture* mpParticleTexture;

	ID3D11Texture2D* mpRenderSurfaceTexture;
	ID3D11ShaderResourceView* mpRenderSurfaceSRV;
	ID3D11RenderTargetView* mpRenderSurfaceRTV;

	//For the moment is a 2D texture that has 1/8th the height and width of the painting surface
	ID3D11Texture2D* mpDensityMapTexture;
	ID3D11ShaderResourceView* mpDensityMapSRV;
	ID3D11UnorderedAccessView* mpDensityMapUAV;

	//Same resolution as density field
	//Reads the density field to determine the direction a 2D vector from each cel needs to point to reach the lowest density area
	ID3D11Texture2D* mpFlowFieldTexture;
	ID3D11ShaderResourceView* mpFlowFieldSRV;
	ID3D11UnorderedAccessView* mpFlowFieldUAV;

	//Same resolution as density field
	//Stores the average velocity of all particles in each cel of a field
	ID3D11Texture2D* mpVelocityFieldTexture;
	ID3D11ShaderResourceView* mpVelocityFieldSRV;
	ID3D11UnorderedAccessView* mpVelocityFieldUAV;

	//Stores the derivative of the density field
	//Not sure if this should used purely for visualization or also be part of the simulation step
	ID3D11Texture2D* mpWavefrontTexture;
	ID3D11ShaderResourceView* mpWavefrontSRV;
	ID3D11UnorderedAccessView* mpWavefrontUAV;

	//Height/width in pixels
	unsigned int mRenderSurfaceResolution;

	//Resolution of fields = surface resolution / this
	unsigned int mFieldResolutionDownsampling;

	//mRenderSurfaceResolution / mFieldResolutionDownsampling
	unsigned int mFieldResolution;
};