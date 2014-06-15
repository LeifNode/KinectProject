#pragma once

#include "d3dStd.h"
#include "PointList.h"

class Shader;
class D3DRenderer;

class LineRenderer
{
	struct CBPerLine
	{
		unsigned int PointCount;
		XMUINT3 pad;
		XMFLOAT4 Color;
		float Thickness;
		XMFLOAT3 pad2;
	};

public:
	LineRenderer();
	~LineRenderer();

	void reloadPoints();

	static void Initialize();
	static void DeInit();

	void Render(D3DRenderer* renderer);

	PointList Points;

private:
	static Shader* pLineShader;
	static ID3D11Buffer* pPerLineBuffer;

	ID3D11Buffer* mpVertexBuffer;
};