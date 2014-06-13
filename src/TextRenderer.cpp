#include "TextRenderer.h"
#include "D3DRenderer.h"
#include "d3dApp.h"
#include "FontManager.h"

TextRenderer::TextRenderer(int capacity)
	:mTextChanged(false),
	mpVertexBuffer(NULL),
	mMaxCharCount(capacity)
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

	pVertArray[0].Enabled = true;
	pVertArray[0].Dimensions = XMFLOAT2(2.0f, 2.0f);
	pVertArray[0].TexTL = XMFLOAT2(0.0f, 0.0f);
	pVertArray[0].TexBR = XMFLOAT2(1.0f, 1.0f);
	
	initData.pSysMem = pVertArray;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = gpApplication->getRenderer()->device()->CreateBuffer(&desc, &initData, &mpVertexBuffer);
	if (FAILED(hr))
		std::cout << "Failed to create text renderer vertex buffer.\n";

	delete [] pVertArray;
}

void TextRenderer::Render(D3DRenderer* renderer)
{
	CBPerObject perObject;

	perObject.World = mTransform.getTransform();
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

	renderer->setPerObjectBuffer(perObject);

	unsigned int stride = sizeof(TextQuadVert), offset = 0;

	renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	renderer->context()->Draw(mText.size(), 0);
}

void TextRenderer::setText(const std::string& text)
{
	if (text.size() < mMaxCharCount)
	{
		mText = text;
		mTextChanged = true;
	}
}