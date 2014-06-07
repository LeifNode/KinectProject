#pragma once

#include "d3dStd.h"
#include <NuiApi.h>

class D3DRenderer;

class KinectRenderer
{
public:
	KinectRenderer();
	~KinectRenderer();

	void Initialize();

	void Update(float dt);
	void Render(D3DRenderer* renderer);

private:


};