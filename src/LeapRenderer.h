#pragma once

#include "d3dStd.h"
#include "LineRenderer.h"
#include "MeshRenderer.h"
#include "Geometry.h"

class D3DRenderer;

class LeapRenderer
{
public:
	LeapRenderer();
	~LeapRenderer();

	void Initialize();

	void Update();
	void Render(D3DRenderer* renderer);

private:
	LineRenderer mLineRenderer;
	MeshRenderer<Vertex> mMesh;
};