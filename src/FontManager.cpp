#include "FontManager.h"
#include "D3DRenderer.h"
#include "Texture.h"
#include "d3dApp.h"

FontManager::FontManager()
	:mpFontTexture(NULL),
	mpSampler(NULL),
	mFace(NULL)
{

}

FontManager::~FontManager()
{
	FT_Done_Face(mFace);
	FT_Error error = FT_Done_FreeType(mftLibrary);

	if (error)
		std::cout << "Failed to unload freetype.\n";

	SAFE_DELETE(mpFontTexture);
	
	ReleaseCOM(mpSampler);
}

void FontManager::Initialize()
{
	memset(&mftLibrary, 0, sizeof(mftLibrary));
	FT_Error error = FT_Init_FreeType(&mftLibrary);

	if (error)
	{
		std::cout << "Failed to initialize freetype.\n";
	}

	initializeTexture();
	initializeSampler();
	initializeShader();

	mBinPacker.Initialize(2048, 2048);
}

void FontManager::initializeTexture()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = 2048;
	textureDesc.Height = 2048;
	textureDesc.MipLevels = 11; //2^10 =1024
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	mpFontTexture = renderer->createTexture(&textureDesc);

	if (mpFontTexture == NULL)
		std::cout << "Failed to create font texture.\n";
}

void FontManager::initializeSampler()
{
	D3DRenderer* renderer = gpApplication->getRenderer();
	ID3D11DeviceContext* context = renderer->context();

	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = renderer->device()->CreateSamplerState(&samplerDesc, &mpSampler);
	if (FAILED(result))
		std::cout << "Failed to create sampler state for font.\n";
}

void FontManager::initializeShader()
{
	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_GEOMETRY, "GS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DIMENSION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	mpTextRenderShader = gpApplication->getRenderer()->loadShader(L"Shaders/text.fx", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, vertexDescription, ARRAYSIZE(vertexDescription)); 
}

void FontManager::loadFont(const std::string fontPath)
{
	if (mFace)
		FT_Done_Face(mFace);
	FT_Error error = FT_New_Face(mftLibrary, fontPath.c_str(), 0, &mFace);

	if (error == FT_Err_Unknown_File_Format)
	{
		std::cout << "Invalid font format." << std::endl;
	}
	else if (error)
	{
		std::cout << "Error loading font.\n";
	}
}

void FontManager::loadCharacter(char ch, int pointSize)
{
	FT_Error error = FT_Set_Char_Size(mFace, 0, pointSize * 64, 300, 300);
	assert(error == 0);

	int glyphIndex = FT_Get_Char_Index(mFace, ch);
	assert(glyphIndex != 0);

	error = FT_Load_Glyph(mFace, glyphIndex, FT_LOAD_DEFAULT);
	assert(error == 0);

	FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_NORMAL);
	assert(mFace->glyph->format == FT_GLYPH_FORMAT_BITMAP);

	FT_Bitmap* bitmap = &mFace->glyph->bitmap;
	assert(bitmap);

	int width = bitmap->width;
	int height = bitmap->rows;

	if (height == 0 || width == 0)
		return;

	auto node = mBinPacker.insert(width + 2, height + 2);

	if (node != NULL)
	{
		D3DRenderer* renderer = gpApplication->getRenderer();

		D3D11_BOX box;
		box.left = node->x + 1;
		box.right = box.left + width;
		box.top = node->y + 1;
		box.bottom = box.top + height;
		box.front = 0;
		box.back = 1;

		renderer->context()->UpdateSubresource(mpFontTexture->getD3DTexture(), D3D11CalcSubresource(0, 0, 11), &box, bitmap->buffer, bitmap->pitch, 0); 
	}
}

void FontManager::loadGlyphs(int ptSize)
{
	for (int i = 33; i < 127; i++)
	{
		loadCharacter((char)i, ptSize);
	}

	gpApplication->getRenderer()->context()->GenerateMips(mpFontTexture->getResourceView());

	std::cout << "Used space: " << mBinPacker.getFillPercent() << "%\n";
}

void FontManager::bindRender(D3DRenderer* renderer)
{
	renderer->setShader(mpTextRenderShader);
	renderer->setTextureResource(0, mpFontTexture);
	renderer->setSampler(0, mpSampler);
}