#pragma once

#include "d3dStd.h"
#include "RectangleBinPacker.h"
#include "Rectangle.h"
#include "Texture.h"
#include <freetype\ft2build.h>
#include FT_FREETYPE_H

enum FONT_TYPE
{
	FONT_TYPE_NORMAL,
	FONT_TYPE_BOLD,
	FONT_TYPE_ITALIC,
	FONT_TYPE_BOLDITALIC,
	FONT_TYPE_COUNT,
};

class Font
{
	friend class FontManager;
public:
	struct Glyph
	{
		int character;
		int pointSize;
		int left;
		int top;
		int width;
		int height;
		int advance;
		int bearing;
	};

public:
	std::string getFontPath(FONT_TYPE type) const { return mFontPaths[type]; }

	Rect measureString(const std::string& str, int pointSize, FONT_TYPE type = FONT_TYPE_NORMAL);
	Rect measureString(const std::string& str, int pointSize, FONT_TYPE type, int verticalOffset, const Rect& bounds);

	const Glyph* getCharacter(int charCode, int pointSize = 12, FONT_TYPE type = FONT_TYPE_NORMAL);

	int getTextureWidth() const { return mpFontTexture->getWidth(); }
	int getTextureHeight() const { return mpFontTexture->getHeight(); }

	bool loadCharacter(int charCode, int pointSize, FONT_TYPE type);
	bool loadGlyphs(int pointSize, FONT_TYPE type);

	float getUsedSpace() { return mBinPacker.getFillPercent(); }
	Texture* getFontTexture() const { return mpFontTexture; }

private:
	Font();
	~Font();

	void InitializeTexture(int width, int height);
	void loadFont(const std::string& path, FT_Library& library, FONT_TYPE type = FONT_TYPE_NORMAL);

private:
	//int for char code and int for point size
	std::map<std::pair<int, int>, Glyph*> mCharacterMaps[FONT_TYPE_COUNT];

	RectangleBinPacker mBinPacker;
	FT_Face mFaces[FONT_TYPE_COUNT];

	int mNumMipmaps;
	int mHorizontalOffset; //Used to avoid characters with short widths from overlapping
	std::string mFontPaths[FONT_TYPE_COUNT];

	Texture* mpFontTexture;
};