#pragma once

#include "d3dStd.h"
#include "RectangleBinPacker.h"
#include <freetype\ft2build.h>
#include "Font.h"
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

	Font* loadFont(const std::string& fontKey, 
				   const std::string& fontPath, 
				   const std::string& fontPathBold = "", 
				   const std::string& fontPathItalic = "", 
				   const std::string& fontPathBoldItalic = "");

	Font* getFont(const std::string& key);

	void bindRender(D3DRenderer* renderer);

private:
	void initializeSampler();
	void initializeShader();

private:
	FT_Library mftLibrary;

	std::map<std::string, Font*> mFontMap;

	Shader* mpTextRenderShader;
	ID3D11SamplerState* mpSampler;
};