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
	struct Glyph
	{
		char character;
		int pointSize;
		int left;
		int top;
		int width;
		int height;
		int advance;
		int bearing;
	};

public:
	FontManager();
	~FontManager();

	void Initialize();

	void loadFont(const std::string fontPath);
	void loadGlyphs(int ptSize = 12);

	const Glyph* getGlyph(char ch, int pointSize);

	void bindRender(D3DRenderer* renderer);

private:
	void initializeTexture();
	void initializeSampler();
	void initializeShader();
	void initializeBlendState();
	
	void loadCharacter(char ch, int pointSize);

private:
	//Character and point size
	std::map<std::pair<char, int>, Glyph*> mGlyphMap;

	RectangleBinPacker mBinPacker;

	FT_Library mftLibrary;
	FT_Face mFace;

	Shader* mpTextRenderShader;
	Texture* mpFontTexture;
	ID3D11SamplerState* mpSampler;
	ID3D11BlendState* mpBlendState;
};