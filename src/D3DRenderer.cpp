#include "D3DRenderer.h"
#include "d3dApp.h"
#include "Geometry.h"
#include "ConstantBuffers.h"

extern D3DApp* gpApplication;

D3DRenderer::D3DRenderer()
	:
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mEnable4xMsaa(true),
	m4xMsaaQuality(16),

	md3dDevice(NULL),
	md3dImmediateContext(NULL),
	mSwapChain(NULL),
	mDepthStencilBuffer(NULL),
	mRenderTarget(NULL),
	mDepthStencilView(NULL),
	mPerFrameBuffer(NULL),
	mPerObjectBuffer(NULL)
{
	mClearColor[0] = 0.0f;
	mClearColor[1] = 0.125f;
	mClearColor[2] = 0.3f;
	mClearColor[3] = 1.0f;

	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
}

D3DRenderer::~D3DRenderer()
{
	for (auto it = mLoadedShaders.begin(); it != mLoadedShaders.end(); ++it)
	{
		delete it->second;
	}
	mLoadedShaders.clear();

	ReleaseCOM(mSamplerState);
	ReleaseCOM(mPerFrameBuffer);
	ReleaseCOM(mPerObjectBuffer);
	ReleaseCOM(mRenderTarget);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	if ( md3dImmediateContext )
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
}

void D3DRenderer::onResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTarget);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, gpApplication->getClientWidth(), gpApplication->getClientHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTarget));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = gpApplication->getClientWidth();
	depthStencilDesc.Height    = gpApplication->getClientHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if( mEnable4xMsaa )
	{
		depthStencilDesc.SampleDesc.Count   = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality-1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTarget, mDepthStencilView);
	

	// Set the viewport transform.

	setViewport(gpApplication->getClientWidth(), gpApplication->getClientHeight(), 0, 0);
}

bool D3DRenderer::initialize()
{
	// Create the device and device context.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
			0,                 // default adapter
			md3dDriverType,
			0,                 // no software device
			createDeviceFlags, 
			0, 0,              // default feature level array
			D3D11_SDK_VERSION,
			&md3dDevice,
			&featureLevel,
			&md3dImmediateContext);

	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if( featureLevel != D3D_FEATURE_LEVEL_11_0 )
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert( m4xMsaaQuality > 0 );

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof( sd ));
	sd.BufferDesc.Width  = gpApplication->getClientWidth();
	sd.BufferDesc.Height = gpApplication->getClientHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if( mEnable4xMsaa )
	{
		sd.SampleDesc.Count   = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality-1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count   = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = gpApplication->mainWnd();
#if USE_RIFT
	sd.Windowed     = false;
#else
	sd.Windowed     = true;
#endif
	//sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	      
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;

	ID3D11RasterizerState* rasterState;

	md3dDevice->CreateRasterizerState(&rasterDesc, &rasterState);
	md3dImmediateContext->RSSetState(rasterState);

	ReleaseCOM(rasterState);

	//Per frame buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBPerFrame);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	md3dDevice->CreateBuffer(&bd, NULL, &mPerFrameBuffer);

	//Per object buffer
	bd.ByteWidth = sizeof(CBPerObject);

	md3dDevice->CreateBuffer(&bd, NULL, &mPerObjectBuffer);

	D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 4;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	md3dDevice->CreateSamplerState( &sampDesc, &mSamplerState );

	onResize();

	return true;
}

HRESULT D3DRenderer::compileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile( szFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

Shader* D3DRenderer::loadShader(WCHAR* filePath, ShaderInfo* shaderInfo, D3D_PRIMITIVE_TOPOLOGY primitiveTopology, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int vertexDescriptionSize)
{
	char* name = new char[MAX_PATH];

	//Convert wchar* to char*
	wcstombs(name, filePath, MAX_PATH);

	std::string nameStr(name);

	delete name;
	name = nullptr;

	Shader* newShader = new Shader(nameStr);
	newShader->mPrimitiveTopology = primitiveTopology;
	mLoadedShaders[nameStr] = newShader;

	HRESULT hr = S_OK;
	ID3DBlob* shaderBlob = nullptr;

	for (int i = 0; shaderInfo[i].type != SHADER_TYPE_NONE; i++)
	{
		shaderBlob = nullptr;

		switch (shaderInfo[i].type)
		{
		case SHADER_TYPE_VERTEX:
			hr = compileShaderFromFile(filePath, shaderInfo[i].entrypoint, "vs_5_0", &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );

				return NULL;
			}

			hr = device()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpVertexShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();
		
				MessageBox(0, L"Unable to create vertex shader.", L"Error", MB_OK);
			}

			//Create the input layout
			hr = md3dDevice->CreateInputLayout(vertexDescription, vertexDescriptionSize, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &newShader->mpInputLayout);
			break;
		case SHADER_TYPE_PIXEL:
			hr = compileShaderFromFile(filePath, shaderInfo[i].entrypoint, "ps_5_0", &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );

				return NULL;
			}

			hr = device()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpPixelShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();
		
				MessageBox(0, L"Unable to create pixel shader.", L"Error", MB_OK);
			}
			break;
		case SHADER_TYPE_GEOMETRY:
			hr = compileShaderFromFile(filePath, shaderInfo[i].entrypoint, "gs_5_0", &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );

				return NULL;
			}

			hr = device()->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpGeometryShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();
		
				MessageBox(0, L"Unable to create geometry shader.", L"Error", MB_OK);
			}
			break;
		case SHADER_TYPE_COMPUTE:
			hr = compileShaderFromFile(filePath, shaderInfo[i].entrypoint, "cs_5_0", &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );

				return NULL;
			}

			hr = device()->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpComputeShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();
		
				MessageBox(0, L"Unable to create compute shader.", L"Error", MB_OK);
			}
			break;
		}

		ReleaseCOM(shaderBlob);
	}

	return newShader;
}

void D3DRenderer::setTextureResource(int index, Texture* texture)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			md3dImmediateContext->VSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasPixelShader())
			md3dImmediateContext->PSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasGeometryShader())
			md3dImmediateContext->GSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasComputeShader())
			md3dImmediateContext->CSSetShaderResources(index, 1, &texture->mpResourceView);
	}
}

void D3DRenderer::setSampler(int index, ID3D11SamplerState* samplerState)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			md3dImmediateContext->VSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasPixelShader())
			md3dImmediateContext->PSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasGeometryShader())
			md3dImmediateContext->GSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasComputeShader())
			md3dImmediateContext->CSSetSamplers(index, 1, &samplerState);
	}
}

void D3DRenderer::setConstantBuffer(int index, ID3D11Buffer* buffer)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			md3dImmediateContext->VSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasPixelShader())
			md3dImmediateContext->PSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasGeometryShader())
			md3dImmediateContext->GSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasComputeShader())
			md3dImmediateContext->CSSetConstantBuffers(index, 1, &buffer);
	}
}

void D3DRenderer::setPerFrameBuffer(CBPerFrame& buffer)
{
	md3dImmediateContext->UpdateSubresource(mPerFrameBuffer, 0, NULL, &buffer, 0, 0);
	setConstantBuffer(0, mPerFrameBuffer);
	mPerFrameData = buffer;
}

void D3DRenderer::setPerObjectBuffer(CBPerObject& buffer)
{
	md3dImmediateContext->UpdateSubresource(mPerObjectBuffer, 0, NULL, &buffer, 0, 0);
	setConstantBuffer(1, mPerObjectBuffer);
}

Texture* D3DRenderer::createTexture(UINT format, int width, int height)
{
	DXGI_FORMAT d3dformat;

	if (format & Texture_RGBA && format & Texture_KinectDynamic)
		d3dformat = DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (format & Texture_RGBA)
		d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (format & Texture_Depth && format & Texture_KinectDynamic)
		d3dformat = DXGI_FORMAT_R16_SINT;
	else if (format & Texture_Depth)
		d3dformat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//d3dformat = DXGI_FORMAT_D32_FLOAT;
	else 
		return NULL;

	Texture* newTexture = new Texture();


	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = d3dformat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (format & Texture_KinectDynamic)
	{
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	if (format & Texture_RenderTarget)
	{
		if ((format & Texture_TypeMask) == Texture_Depth)
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		else
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	HRESULT result = md3dDevice->CreateTexture2D(&textureDesc, NULL, &newTexture->mpTexture);
	if (FAILED(result))
	{
		delete newTexture;
		return NULL;
	}

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		result = md3dDevice->CreateShaderResourceView(newTexture->mpTexture, &shaderResourceViewDesc, &newTexture->mpResourceView);
		if (FAILED(result))
		{
			delete newTexture;
			return NULL;
		}
	}

	return newTexture;
}

Texture* D3DRenderer::createTexture(D3D11_TEXTURE2D_DESC* textureDescription)
{
	Texture* newTexture = new Texture();
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	shaderResourceViewDesc.Format = textureDescription->Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDescription->MipLevels;

	HRESULT result = md3dDevice->CreateTexture2D(textureDescription, NULL, &newTexture->mpTexture);
	if (FAILED(result))
	{
		delete newTexture;
		return NULL;
	}

	result = md3dDevice->CreateShaderResourceView(newTexture->mpTexture, &shaderResourceViewDesc, &newTexture->mpResourceView);
	if (FAILED(result))
	{
		delete newTexture;
		return NULL;
	}

	return newTexture;
}

RenderTarget* D3DRenderer::createRenderTarget(int width, int height, bool useDepthBuffer)
{
	RenderTarget* newRenderTarget = new RenderTarget();
	newRenderTarget->mpRenderTargetTexture = createTexture(Texture_RenderTarget | Texture_RGBA, width, height);

	if (useDepthBuffer)
		newRenderTarget->mpDepthTexture = createTexture(Texture_RenderTarget | Texture_Depth, width, height);

	newRenderTarget->mWidth = width;
	newRenderTarget->mHeight = height;
	
	HRESULT result;

	D3D11_RENDER_TARGET_VIEW_DESC renderViewDesc;

	renderViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderViewDesc.Texture2D.MipSlice = 0;

	result = md3dDevice->CreateRenderTargetView(newRenderTarget->mpRenderTargetTexture->mpTexture, &renderViewDesc, &newRenderTarget->mpRenderTargetView);
	if (FAILED(result))
	{
		delete newRenderTarget;
		return NULL;
	}

	if (useDepthBuffer)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;

		result = md3dDevice->CreateDepthStencilView(newRenderTarget->mpDepthTexture->mpTexture, NULL, &newRenderTarget->mpDepthView);
		if (FAILED(result))
		{
			delete newRenderTarget;
			return NULL;
		}
	}

	return newRenderTarget;
}

void D3DRenderer::setRenderTarget(RenderTarget* target)
{
	md3dImmediateContext->OMSetRenderTargets(1, &target->mpRenderTargetView, target->mpDepthView);
}

void D3DRenderer::setViewport(int width, int height, int x, int y)
{
	mScreenViewport.TopLeftX = (float)x;
	mScreenViewport.TopLeftY = (float)y;
	mScreenViewport.Width    = (float)width;
	mScreenViewport.Height   = (float)height;
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void D3DRenderer::setShader(Shader* shader)
{
	shader->bind(this);
	mpActiveShader = shader;
}

void D3DRenderer::setShader(const std::string& name)
{
	auto it = mLoadedShaders.find(name);

	if (it != mLoadedShaders.end())
		setShader(it->second);
	else
		throw -1;
}

Shader* D3DRenderer::getShader(const std::string& name)
{
	auto it = mLoadedShaders.find(name);

	if (it != mLoadedShaders.end())
	{
		return it->second;
	}

	return nullptr;
}

void D3DRenderer::clear(RenderTarget* target)
{
	md3dImmediateContext->ClearRenderTargetView(target->mpRenderTargetView, mClearColor);
	md3dImmediateContext->ClearDepthStencilView(target->mpDepthView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3DRenderer::preRender()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTarget, mClearColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->VSSetSamplers(0, 1, &mSamplerState);
	md3dImmediateContext->PSSetSamplers(0, 1, &mSamplerState);
	md3dImmediateContext->GSSetSamplers(0, 1, &mSamplerState);
}

void D3DRenderer::postRender()
{
#if !USE_RIFT
	mSwapChain->Present(0, 0);
#endif

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTarget, mDepthStencilView);
}