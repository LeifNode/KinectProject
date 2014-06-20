#pragma once

#include "d3dStd.h"
#include "Transform.h"

class D3DRenderer;
class Font;

struct TextQuadVert
{
	XMFLOAT3 Position;
	XMFLOAT2 TexTL;
	XMFLOAT2 TexBR;
	XMFLOAT2 Dimensions;
};

class TextRenderer
{
public:
	TextRenderer(int capacity = 500);
	~TextRenderer();

	void Initialize();

	void Render(D3DRenderer* renderer);

	void setTextWrap(bool enabled) { mTextWrapEnabled = enabled; }
	bool getTextWrap() const { return mTextWrapEnabled; }

	void setText(const std::string& text);
	std::string getText() const { return mText; }
	int getCharCount() const { return (int)mText.size(); }
	int getMaxCharCount() const { return mMaxCharCount; }

	void setFont(Font* font) { mpFont = font; mTextChanged = true; }
	Font* getFont() const { return mpFont; }

	int getTextSize() const { return mTextSize; }
	void setTextSize(int size) { mTextSize = size; }

private:
	void updateVertexBuffer();

public:
	Transform mTransform;

private:
	Font* mpFont;

	bool mTextWrapEnabled;
	float mMaxWidth;
	float mVerticalSpacing;

	int mMaxCharCount;
	int mTextSize;

	bool mTextChanged;
	std::string mText;

	ID3D11Buffer* mpVertexBuffer;
};