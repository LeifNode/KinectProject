#pragma once

#include "d3dStd.h"
#include "Transform.h"

class D3DRenderer;

struct TextQuadVert
{
	XMFLOAT3 Position;
	XMFLOAT2 TexTL;
	XMFLOAT2 TexBR;
	XMFLOAT2 Dimensions;
	bool Enabled;
};

class TextRenderer
{
public:
	TextRenderer(int capacity = 500);
	~TextRenderer();

	void Initialize();

	void Render(D3DRenderer* renderer);

	void setText(const std::string& text);
	std::string getText() const { return mText; }

public:
	Transform mTransform;

private:
	int mMaxCharCount;

	bool mTextChanged;
	std::string mText;

	ID3D11Buffer* mpVertexBuffer;
};