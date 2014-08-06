#pragma once

#include "d3dStd.h"
#include "Shader.h"
#include "ConstantBuffers.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "MatrixStack.h"
#include "GBuffer.h"
#include "Transform.h"

class D3DApp;


class D3DRenderer
{
	friend class OVRRenderer;

public:
	enum Depth_Stencil_State
	{
		Depth_Stencil_State_Invalid = -1,
		Depth_Stencil_State_Default,
		Depth_Stencil_State_Particle,
		Depth_Stencil_State_Count,
	};

public:
	D3DRenderer();
	~D3DRenderer();

	bool initialize();
	void onResize();

	ID3D11Device* device() const { return md3dDevice; }
	ID3D11DeviceContext* context() const { return md3dImmediateContext; }

	void setTextureResource(int index, Texture*);
	void setTextureResources(int index, Texture** texArray, unsigned count);
	void setSampler(int index, ID3D11SamplerState* samplerState);
	void setConstantBuffer(int index, ID3D11Buffer*);
	void setPerFrameBuffer(CBPerFrame& buffer);
	void setPerObjectBuffer(CBPerObject& buffer);

	Texture* createTexture(UINT format, int width, int height);
	Texture* createTexture(D3D11_TEXTURE2D_DESC* textureDescription, DXGI_FORMAT resViewFmt = DXGI_FORMAT_UNKNOWN);
	//Texture* createTexture(int width, int height, DXGI_FORMAT colorFormat, )
	RenderTarget* createRenderTarget(int width, int height, bool useDepthBuffer = true);
	void setRenderTarget(RenderTarget* target);

	void setBlendState(bool blendingEnabled);
	void setDepthStencilState(Depth_Stencil_State state);

	void setViewport(int width, int height, int x, int y);

	const CBPerFrame* getPerFrameBuffer() const { return &mPerFrameData; }

	void setShader(Shader* shader);
	void setShader(const std::string& name);
	Shader* getShader(const std::string& name);
	Shader* getActiveShader() const { return mpActiveShader; }

	void resetSamplerState();

	Shader* loadShader(WCHAR* filePath, ShaderInfo* shaderInfo, D3D_PRIMITIVE_TOPOLOGY primitiveTopology, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int vertexDescriptionSize);

	void clear(RenderTarget* target);

	void preRender();
	void postRender();

	void pushTransform(Transform& transform);
	void popTransform();
	XMMATRIX getTopTransform() const;
	XMMATRIX getTopTransformInverse() const;

private:
	static HRESULT compileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

	void initializeDepthStencilStates();

private:
	GBuffer* mpGBuffer;

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
	CBPerFrame mPerFrameData;
	ID3D11Buffer* mPerObjectBuffer;
	ID3D11SamplerState* mSamplerState;
	ID3D11BlendState* mBlendStateAlpha;
	ID3D11BlendState* mBlendStateOpaque;

	ID3D11DepthStencilState* mDepthStencilStates[Depth_Stencil_State_Count];

	bool mEnable4xMsaa;

	float mClearColor[4];

	Shader* mpActiveShader;

	std::map<std::string, Shader*> mLoadedShaders;

	MatrixStack mMatrixStack;
};