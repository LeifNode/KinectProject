#pragma once

#include "d3dStd.h"
#include "Lights.h"

struct CBPerObject
{
	XMMATRIX World;
	XMMATRIX WorldViewProj;
	XMMATRIX WorldInvTranspose;
	XMMATRIX TextureTransform;
	Material Material;
};

struct CBPerFrame
{
	XMMATRIX View;
	XMMATRIX ViewInv;
	XMMATRIX Projection;
	XMMATRIX ProjectionInv;

	XMMATRIX ViewProj;
	XMMATRIX ViewProjInv;

	XMFLOAT3 HeadPosition;
	float pad;
	XMFLOAT3 EyePosition;
	float pad2;
	XMFLOAT3 EyeDirection;
	float pad3;

	DirectionalLight DirectionalLight;
	PointLight PointLight;
	SpotLight SpotLight;
};