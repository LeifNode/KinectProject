#pragma once

#include "d3dStd.h"
#include "RectangleBinPacker.h"
#include <freetype\ft2build.h>
#include FT_FREETYPE_H

class Texture;
class Shader;
class D3DRenderer;

class FontManager
{
public:
	FontManager();
	~FontManager();

	void Initialize();

	void loadFont(const std::string fontPath);
	void loadGlyphs(int ptSize = 12);

	void bindRender(D3DRenderer* renderer);

private:
	void initializeTexture();
	void initializeSampler();
	void initializeShader();

	void loadCharacter(char ch, int pointSize);

private:
	RectangleBinPacker mBinPacker;

	FT_Library mftLibrary;
	FT_Face mFace;

	Shader* mpTextRenderShader;
	Texture* mpFontTexture;
	ID3D11SamplerState* mpSampler;
	ID3D11BlendState* mpBlendState;
};