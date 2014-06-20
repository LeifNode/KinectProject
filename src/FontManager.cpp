#include "FontManager.h"
#include "D3DRenderer.h"
#include "Texture.h"
#include "d3dApp.h"

FontManager::FontManager()
	:mpSampler(NULL)
{

}

FontManager::~FontManager()
{
	FT_Error error = FT_Done_FreeType(mftLibrary);

	if (error)
		std::cout << "Failed to unload freetype.\n";
	
	ReleaseCOM(mpSampler);

	for (auto it = mFontMap.begin(); it != mFontMap.end(); ++it)
	{
		delete it->second;
	}
}

void FontManager::Initialize()
{
	memset(&mftLibrary, 0, sizeof(mftLibrary));
	FT_Error error = FT_Init_FreeType(&mftLibrary);

	if (error)
	{
		std::cout << "Failed to initialize freetype.\n";
	}
	
	initializeSampler();
	initializeShader();
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

void FontManager::bindRender(D3DRenderer* renderer)
{
	renderer->setShader(mpTextRenderShader);
	renderer->setSampler(0, mpSampler);
}

Font* FontManager::loadFont(const std::string& fontKey, 
							const std::string& fontPath, 
							const std::string& fontPathBold, 
							const std::string& fontPathItalic, 
							const std::string& fontPathBoldItalic)
{
	auto it = mFontMap.find(fontKey);
	if (it != mFontMap.end())
	{
		std::cout << "Font already loaded.\n";
		return it->second;
	}

	Font* newFont = new Font();
	newFont->InitializeTexture(2048, 2048);

	newFont->loadFont(fontPath, mftLibrary);

	if (fontPathBold != "")
		newFont->loadFont(fontPathBold, mftLibrary, FONT_TYPE_BOLD);
	if (fontPathItalic != "")
		newFont->loadFont(fontPathItalic, mftLibrary, FONT_TYPE_ITALIC);
	if (fontPathBoldItalic != "")
		newFont->loadFont(fontPathBoldItalic, mftLibrary, FONT_TYPE_BOLDITALIC);

	mFontMap.insert(std::make_pair(fontKey, newFont));

	return newFont;
}

Font* FontManager::getFont(const std::string& key)
{
	auto it = mFontMap.find(key);

	if (it != mFontMap.end())
		return it->second;

	return NULL;
}