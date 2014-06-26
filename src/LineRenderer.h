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
	enum LINE_DRAW_MODE
	{
		LINE_DRAW_MODE_LINELIST,
		LINE_DRAW_MODE_LINESTRIP,
		LINE_DRAW_MODE_COUNT,
	};

public:
	LineRenderer();
	~LineRenderer();

	void reloadPoints();

	void setDrawMode(LINE_DRAW_MODE mode);

	static void Initialize();
	static void DeInit();

	void Render(D3DRenderer* renderer);

	PointList Points;

private:
	void generateIndices(UINT** ppArrayOut, UINT* pIndexCount);

private:
	static Shader* pLineShader;
	static ID3D11Buffer* pPerLineBuffer;

	ID3D11Buffer* mpVertexBuffer;
	ID3D11Buffer* mpIndexBuffer;

	LINE_DRAW_MODE mMode;
};