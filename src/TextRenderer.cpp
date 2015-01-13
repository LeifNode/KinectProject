#include "TextRenderer.h"
#include "D3DRenderer.h"
#include "d3dApp.h"
#include "FontManager.h"
#include "Font.h"

TextRenderer::TextRenderer(int capacity)
	:mTextChanged(false),
	mpVertexBuffer(NULL),
	mMaxCharCount(capacity),
	mpFont(NULL),
	mTextSize(12)
{
}

TextRenderer::~TextRenderer()
{
	ReleaseCOM(mpVertexBuffer);
}

void TextRenderer::Initialize()
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA initData;

	ZeroMemory(&desc, sizeof(desc));

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(TextQuadVert) * mMaxCharCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	TextQuadVert* pVertArray = new TextQuadVert[mMaxCharCount]();
	memset(pVertArray, 0, sizeof(TextQuadVert) * mMaxCharCount);//Clear array

	pVertArray[0].Dimensions = XMFLOAT2(2.0f, 2.0f);
	pVertArray[0].TexTL = XMFLOAT2(0.0f, 0.0f);
	pVertArray[0].TexBR = XMFLOAT2(1.0f, 1.0f);
	
	initData.pSysMem = pVertArray;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = gpApplication->getRenderer()->device()->CreateBuffer(&desc, &initData, &mpVertexBuffer);
	if (FAILED(hr))
		std::cout << "Failed to create text renderer vertex buffer.\n";

	SAFE_DELETEARR(pVertArray);
}

void TextRenderer::updateVertexBuffer()
{
	if (mTextChanged)
	{
		FontManager* fontManager = gpApplication->getFontManager();
		D3DRenderer* renderer = gpApplication->getRenderer();

		TextQuadVert* pVertArray = new TextQuadVert[mMaxCharCount]();
		memset(pVertArray, 0, sizeof(TextQuadVert) * mMaxCharCount);//Clear array
		float textOffset = 0.0f;
		float heightOffset = 0.4f;

		int skippedCount = 0;
		int lineCount = 0;

		for (int i = 0; i < mText.size(); i++)
		{
			char ch = mText[i];
			lineCount++;
			if (ch == '\n')
			{
				textOffset = 0;
				heightOffset -= 0.018f;
				skippedCount++;
				continue;
			}

			if (lineCount > 450)
			{
				textOffset = 0;
				heightOffset -= 0.018f;
				lineCount = 0;
			}
			
			const Font::Glyph* glyph = mpFont->getCharacter(ch, mTextSize);

			if (glyph == NULL) //Character not loaded
			{
				std::cout << "Char null" << std::endl;
				skippedCount++;
				continue;
			}

			int index = i - skippedCount;
			float width = (float)mpFont->getTextureWidth();
			float height = (float)mpFont->getTextureHeight();
			
			pVertArray[index].TexTL = XMFLOAT2(glyph->left / width, glyph->top / height);
			pVertArray[index].TexBR = XMFLOAT2((glyph->left + glyph->width) / width, (glyph->top + glyph->height) / height);
			pVertArray[index].Dimensions = XMFLOAT2(glyph->width / 12000.0f, glyph->height / 12000.0f);//Transform pixel counts into meters assuming 300dpi
			pVertArray[index].Position = XMFLOAT3(textOffset, heightOffset - (glyph->height - glyph->bearing) / 12000.0f, 0.0f);

			textOffset += (glyph->advance + 2) / 12000.0f;
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		renderer->context()->Map(mpVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		memcpy(resource.pData, pVertArray, sizeof(TextQuadVert) * mMaxCharCount);

		renderer->context()->Unmap(mpVertexBuffer, 0);

		SAFE_DELETEARR(pVertArray);

		mTextChanged = false;
	}
}

void TextRenderer::Render(D3DRenderer* renderer)
{
	updateVertexBuffer();

	renderer->setTextureResource(0, mpFont->getFontTexture());

	CBPerObject perObject;

	//perObject.World =  XMMatrixScaling(-1.0f, 1.0f, 1.0f) * mTransform.getTransform();
	XMVECTOR worldOffset = XMLoadFloat3(&renderer->getPerFrameBuffer()->HeadPosition);
	perObject.World = XMMatrixScaling(-1.0f, 1.0f, 1.0f) * mTransform.getTransform() * XMMatrixTranslation(0.25f, -0.2f, 0.3f) * XMMatrixTranslationFromVector(worldOffset);
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;
	//perObject.WorldViewProj = perObject.World * XMMatrixOrthographicLH(1.0f, 0.5625f, 0.0f, 1.0f);

	renderer->setPerObjectBuffer(perObject);

	unsigned int stride = sizeof(TextQuadVert), offset = 0;
	
	renderer->setBlendState(true);
	renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	renderer->context()->Draw((UINT)mText.size(), 0);
}

void TextRenderer::setText(const std::string& text)
{
	if (text.size() < mMaxCharCount && mText != text)
	{
		mTextChanged = true;
		mText = text;
	}
}