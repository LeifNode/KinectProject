#include "LeapRenderer.h"
#include "LeapManager.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

using namespace Leap;

struct CBLeap
{
	XMFLOAT2 RayScale;
	XMFLOAT2 RayOffset;
	float HandDistance;
	float OffsetX;
	XMFLOAT2 Pad;
};

LeapRenderer::LeapRenderer()
	:mpImageShader(NULL),
	mpLeapConstantBuffer(NULL)
{
	mBrightnessTextures[0] = NULL;
	mBrightnessTextures[1] = NULL;

	mBrightnessResourceViews[0] = NULL;
	mBrightnessResourceViews[1] = NULL;

	mDistortionTextures[0] = NULL;
	mDistortionTextures[1] = NULL;

	mDistortionResourceViews[0] = NULL;
	mDistortionResourceViews[1] = NULL;

	mHandMaskTextures[0] = NULL;
	mHandMaskTextures[1] = NULL;

	mHandMaskRSV[0] = NULL;
	mHandMaskRSV[1] = NULL;
}

LeapRenderer::~LeapRenderer()
{
	ReleaseCOM(mBrightnessTextures[0]);
	ReleaseCOM(mBrightnessTextures[1]);
	ReleaseCOM(mBrightnessResourceViews[0]);
	ReleaseCOM(mBrightnessResourceViews[1]);

	ReleaseCOM(mDistortionTextures[0]);
	ReleaseCOM(mDistortionTextures[1]);
	ReleaseCOM(mDistortionResourceViews[0]);
	ReleaseCOM(mDistortionResourceViews[1]);

	ReleaseCOM(mHandMaskTextures[0]);
	ReleaseCOM(mHandMaskTextures[1]);
	ReleaseCOM(mHandMaskRSV[0]);
	ReleaseCOM(mHandMaskRSV[1]);
	
	ReleaseCOM(mpLeapConstantBuffer);
}

void LeapRenderer::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	mpImageShader = renderer->loadShader(L"Shaders/leap.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, NULL, 0);

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width = 640;
	texDesc.Height = 240;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mBrightnessTextures[0]));
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mBrightnessTextures[1]));
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mHandMaskTextures[0]));
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mHandMaskTextures[1]));
	

	texDesc.Width = 64;
	texDesc.Height = 64;
	texDesc.Format = DXGI_FORMAT_R32G32_FLOAT;

	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mDistortionTextures[0]));
	HR(renderer->device()->CreateTexture2D(&texDesc, NULL, &mDistortionTextures[1]));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	HR(renderer->device()->CreateShaderResourceView(mBrightnessTextures[0], &srvDesc, &mBrightnessResourceViews[0]));
	HR(renderer->device()->CreateShaderResourceView(mBrightnessTextures[1], &srvDesc, &mBrightnessResourceViews[1]));
	HR(renderer->device()->CreateShaderResourceView(mHandMaskTextures[0], &srvDesc, &mHandMaskRSV[0]));
	HR(renderer->device()->CreateShaderResourceView(mHandMaskTextures[1], &srvDesc, &mHandMaskRSV[1]));

	srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;

	HR(renderer->device()->CreateShaderResourceView(mDistortionTextures[0], &srvDesc, &mDistortionResourceViews[0]));
	HR(renderer->device()->CreateShaderResourceView(mDistortionTextures[1], &srvDesc, &mDistortionResourceViews[1]));

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBLeap);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	renderer->device()->CreateBuffer(&bufferDesc, NULL, &mpLeapConstantBuffer);

}

void LeapRenderer::updateImages()
{
	D3DRenderer* renderer = gpApplication->getRenderer();
	ID3D11DeviceContext* context = renderer->context();

	const Frame* frame = LeapManager::getInstance().getFrame();

	ImageList images = frame->images();

	for (int i = 0; i < 2; i++)
	{
		Leap::Image image = images[i];

		const unsigned char* imageBuffer = image.data();
		int width = image.width();
		int height = image.height();

		if (imageBuffer == NULL)
			break;

		D3D11_MAPPED_SUBRESOURCE brightnessTexMem;
		HR(context->Map(mBrightnessTextures[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &brightnessTexMem));
		
		if (height == 120) //Robust mode, have to do more special copy
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					*((unsigned char*)brightnessTexMem.pData + 2 * y * brightnessTexMem.RowPitch + x) = imageBuffer[y * brightnessTexMem.RowPitch + x];
					*((unsigned char*)brightnessTexMem.pData + 2 * y * brightnessTexMem.RowPitch + brightnessTexMem.RowPitch + x) = imageBuffer[y * brightnessTexMem.RowPitch + x];
				}
			}
		}
		else
		{
			for (int x = 0; x < width * height; x++)
			{
				*((unsigned char*)brightnessTexMem.pData + x) = imageBuffer[x];
			}
		}

		context->Unmap(mBrightnessTextures[i], 0);

		const float* distortionBuffer = image.distortion();

		D3D11_MAPPED_SUBRESOURCE distortionTexMemory;
		HR(context->Map(mDistortionTextures[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &distortionTexMemory));

		width = image.distortionWidth() / 2;
		height = image.distortionHeight();

		/*for (int x = 0; x < width * height * 2; x++)
		{
			*(distortionTexMemory.pData + x) = distortionBuffer[x];
		}*/

		/*for (int d = 0; d < width * 2 * height; d += 4) 
		{
			float dX = ((float*)distortionBuffer)[d];
			float dY = ((float*)distortionBuffer)[d + 1];
			if(!((dX < 0) || (dX > 1)) && !((dY < 0) || (dY > 1))) 
			{
				*((float*)distortionTexMemory.pData + d) = dX;
				*((float*)distortionTexMemory.pData + d + 1) = dY;
			}
		}*/

		memcpy(distortionTexMemory.pData, distortionBuffer, width * height * 2 * 4);

		context->Unmap(mDistortionTextures[i], 0);
	}
}

void LeapRenderer::buildHandMasks()
{

}

void LeapRenderer::Update()
{
	updateImages();

	LeapManager& leapManager = LeapManager::getInstance();

	mLineRenderer.Points.clear();

	HandList hands = leapManager.getFrame()->hands();

	for (auto handit = hands.begin(); handit != hands.end(); ++handit)
	{
		const Hand hand = *handit;

		const FingerList fingers = hand.fingers();
		for (auto fingerIt = fingers.begin(); fingerIt != fingers.end(); ++fingerIt)
		{
			const Finger finger = *fingerIt;

			for (int b = 0; b < 4; b++)
			{
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				
				mLineRenderer.Points.addPoint(leapManager.transformPosition(bone.prevJoint()));
				mLineRenderer.Points.addPoint(leapManager.transformPosition(bone.nextJoint()));
			}
		}
	}

	mLineRenderer.reloadPoints();
}

void LeapRenderer::Render(D3DRenderer* renderer, int index)
{
	//mLineRenderer.Render(renderer);

	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	//renderer->context()->IASetVertexBuffers(0, 1, NULL, NULL, NULL);

	renderer->setShader(mpImageShader);

	HandList hands = LeapManager::getInstance().getFrame()->hands();

	auto handit = hands.begin();

	CBLeap leapConstants;

	if (handit != hands.end())
	{
		const Hand hand = *handit;	
		leapConstants.HandDistance = (hand.palmPosition().y + 60.0f) / 1000.0f;
		//leapConstants.HandDistance = (hand.finger(1).tipPosition().y + 60.0f) / 1000.0f;
	}
	else
		leapConstants.HandDistance = 1.0f;

	leapConstants.RayOffset = XMFLOAT2(0.5f, 0.5f);
	leapConstants.RayScale = XMFLOAT2(0.125f, 0.125f);
	leapConstants.OffsetX = index == 0 ? -0.0165f : 0.0165f;//Adjusting IPD of camera images
	//leapConstants.OffsetX = index == 0 ? -0.03f : 0.03f;

	renderer->context()->UpdateSubresource(mpLeapConstantBuffer, 0, NULL, &leapConstants, 0, 0);
	renderer->setConstantBuffer(2, mpLeapConstantBuffer);

	renderer->context()->PSSetShaderResources(0, 1, &mBrightnessResourceViews[index]);
	renderer->context()->PSSetShaderResources(1, 1, &mDistortionResourceViews[index]);
	renderer->bindPerFrameBuffer();
	renderer->resetSamplerState();
	

	renderer->context()->Draw(4, 0);
}