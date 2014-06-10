#pragma once

#include "d3dStd.h"

class D3DRenderer;
template <class T>
class MeshRenderer;
struct Vertex;

class HydraRenderer
{
public:
	HydraRenderer();
	~HydraRenderer();

	void Initialize();

	void Render(D3DRenderer* renderer);

private:
	MeshRenderer<Vertex>* mpPointerRenderer;
	MeshRenderer<Vertex>* mpRootRenderer;
};