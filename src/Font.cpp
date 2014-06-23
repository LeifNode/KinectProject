#include "Font.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "MathHelper.h"


Font::Font()
	:mpFontTexture(NULL)
{
	mHorizontalOffset = 2;
	mNumMipmaps = 0;

	for(int i = 0; i < FONT_TYPE_COUNT; i++)
		mFaces[i] = NULL;

	mBinPacker.Initialize(2048, 2048);
}

Font::~Font()
{
	SAFE_DELETE(mpFontTexture);

	for(int i = 0; i < FONT_TYPE_COUNT; i++)
	{
		//if (mFaces[i] != NULL)
			//FT_Done_Face(mFaces[i]);

		auto end = mCharacterMaps[i].end();
		for (auto it = mCharacterMaps[i].begin(); it != end; ++it)
		{
			delete it->second;
		}

		mCharacterMaps[i].clear();
	}
}

void Font::InitializeTexture(int width, int height)
{
	SAFE_DELETE(mpFontTexture);

	D3DRenderer* renderer = gpApplication->getRenderer();

	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = MathHelper::MipMapCount(width, height);
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	mNumMipmaps = textureDesc.MipLevels;

	mpFontTexture = renderer->createTexture(&textureDesc);

	if (mpFontTexture == NULL)
		std::cout << "Failed to create font texture.\n";
}

void Font::loadFont(const std::string& path, FT_Library& library, FONT_TYPE type)
{
	if (mFaces[type])
		FT_Done_Face(mFaces[type]);

	FT_Error error = FT_New_Face(library, path.c_str(), 0, &mFaces[type]);

	if (error == FT_Err_Unknown_File_Format)
	{
		std::cout << "Invalid font format." << std::endl;
	}
	else if (error)
	{
		std::cout << "Error loading font.\n";
	}

	mFontPaths[type] = path;
}

bool Font::loadCharacter(int charCode, int pointSize, FONT_TYPE type)
{
	if (mFaces[type] == NULL)
	{
		std::cout << "Font type not loaded defaulting to normal font\n";
		type = FONT_TYPE_NORMAL;
	}

	if (mCharacterMaps[type].find(std::make_pair(charCode, pointSize)) != mCharacterMaps[type].end())
	{
		std::cout << "Character '" << (char)charCode << "' is already loaded\n";
		return false;
	}

	FT_Error error = FT_Set_Char_Size(mFaces[type], 0, pointSize * 64, 300, 300);
	assert(error == 0);

	int glyphIndex = FT_Get_Char_Index(mFaces[type], charCode);
	assert(glyphIndex != 0);

	error = FT_Load_Glyph(mFaces[type], glyphIndex, FT_LOAD_DEFAULT);
	assert(error == 0);

	FT_Render_Glyph(mFaces[type]->glyph, FT_RENDER_MODE_NORMAL);
	/*assert(mFace->glyph->format == FT_GLYPH_FORMAT_BITMAP);*/

	FT_Bitmap* bitmap = &mFaces[type]->glyph->bitmap;
	assert(bitmap);

	int width = bitmap->width;
	int height = bitmap->rows;

	const int padding = 4;

	auto node = mBinPacker.insert(width + padding, height + padding);

	if (node != NULL)
	{
		//Render to texture
		D3DRenderer* renderer = gpApplication->getRenderer();

		D3D11_BOX box;
		box.left = node->x + padding / 2;
		box.right = box.left + width;
		box.top = node->y + padding / 2;
		box.bottom = box.top + height;
		box.front = 0;
		box.back = 1;

		if (width > 0 && height > 0)
			renderer->context()->UpdateSubresource(mpFontTexture->getD3DTexture(), D3D11CalcSubresource(0, 0, mNumMipmaps), &box, bitmap->buffer, bitmap->pitch, 0); 

		//Cache to map
		Glyph* newGlyph = new Glyph;

		newGlyph->character = charCode;
		newGlyph->pointSize = pointSize;
		newGlyph->left = box.left;
		newGlyph->top = box.top;
		newGlyph->width = width;
		newGlyph->height = height;
		newGlyph->advance = mFaces[type]->glyph->advance.x / 64;
		newGlyph->bearing = mFaces[type]->glyph->metrics.horiBearingY / 64;
		

		mCharacterMaps[type].insert(std::make_pair(std::make_pair(charCode, pointSize), newGlyph));
	}
	else
	{
		std::cout << "Failed to pack character " << (char)charCode << " (" << (int)charCode << ")" << ".\n";
		return false;
	}

	return true;
}

bool Font::loadGlyphs(int pointSize, FONT_TYPE type)
{
	for (int i = 32; i < 127; i++)
	{
		if (!loadCharacter(i, pointSize, type))
			return false;
	}

	gpApplication->getRenderer()->context()->GenerateMips(mpFontTexture->getResourceView());

	return true;
}

Rect Font::measureString(const std::string& str, int pointSize, FONT_TYPE type)
{
	return measureString(str, pointSize, type, 0, Rect());
}

Rect Font::measureString(const std::string& str, int pointSize, FONT_TYPE type, int verticalOffset, const Rect& bounds)
{
	Rect totalSize;

	int size = str.size();
	int maxCharHeight = 0;

	for (int i = 0; i < size; i++)
	{
		const Glyph* character = getCharacter(str[i], pointSize, type);

		if (character == NULL)
			continue;

		int horizontalOffset = character->advance + mHorizontalOffset;

		if (bounds.width == 0 || horizontalOffset + totalSize.width < bounds.width)
		{
			totalSize.width += horizontalOffset;

			maxCharHeight = MathHelper::Max(maxCharHeight, character->height - character->bearing);
		}
		else
		{
			maxCharHeight = 0;
			totalSize.height += verticalOffset;
		}
	}

	totalSize.height += maxCharHeight;//For any characters on the bottom line

	return totalSize;
}

const Font::Glyph* Font::getCharacter(int charCode, int pointSize, FONT_TYPE type)
{
	auto it = mCharacterMaps[type].find(std::make_pair(charCode, pointSize));

	if (it != mCharacterMaps[type].end())
		return it->second;

	return NULL;
}