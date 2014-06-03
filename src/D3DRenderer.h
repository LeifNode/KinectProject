#pragma once

#include "d3dStd.h"
#include "Shader.h"
#include "ConstantBuffers.h"

class D3DApp;
class Texture;

class D3DRenderer
{
public:
	D3DRenderer();
	~D3DRenderer();

	bool initialize();
	void onResize();

	ID3D11Device* device() const { return md3dDevice; }
	ID3D11DeviceContext* context() const { return md3dImmediateContext; }

	void setCameraParameters(XMFLOAT3& cameraPosition, XMFLOAT3& cameraDirection);

	void setTextureResource(int index, Texture*);
	void setConstantBuffer(int index, ID3D11Buffer*);
	void setPerFrameBuffer(CBPerFrame& buffer);
	void setPerObjectBuffer(CBPerObject& buffer);

	void setShader(Shader* shader);
	void setShader(const std::string& name);
	Shader* getShader(const std::string& name);
	Shader* getActiveShader() const { return mpActiveShader; }

	Shader* loadShader(WCHAR* filePath, ShaderInfo* shaderInfo, D3D_PRIMITIVE_TOPOLOGY primitiveTopology, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int vertexDescriptionSize);

	void preRender();
	void postRender();

private:
	static HRESULT compileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

private:
	UINT m4xMsaaQuality;

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTarget;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;
	
	D3D_DRIVER_TYPE md3dDriverType;

	ID3D11Buffer* mPerFrameBuffer;
	ID3D11Buffer* mPerObjectBuffer;
	ID3D11SamplerState* mSamplerState;

	bool mEnable4xMsaa;

	float mClearColor[4];

	Shader* mpActiveShader;

	std::map<std::string, Shader*> mLoadedShaders;
};