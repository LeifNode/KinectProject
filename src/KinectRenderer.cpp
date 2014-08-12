#include "KinectRenderer.h"
#include "D3DRenderer.h"
#include "d3dApp.h"

KinectRenderer::KinectRenderer()
{
	mpNuiSensor = NULL;

	mpVertexBuffer = NULL;
	mpKinectConstantBuffer = NULL;

	mpColorTexture = NULL;
	mpDepthTexture = NULL;
	mpKinectRenderShader = NULL;

	mhNextDepthFrameEvent = INVALID_HANDLE_VALUE;
    mpDepthStreamHandle = INVALID_HANDLE_VALUE;
    mhNextColorFrameEvent = INVALID_HANDLE_VALUE;
    mpColorStreamHandle = INVALID_HANDLE_VALUE;

	DWORD width, height;

	NuiImageResolutionToSize(NUI_DEPTH_RESOLUTION, width, height);
    mDepthWidth  = static_cast<LONG>(width);
    mDepthHeight = static_cast<LONG>(height);

    NuiImageResolutionToSize(NUI_COLOR_RESOLUTION, width, height);
    mColorWidth  = static_cast<LONG>(width);
    mColorHeight = static_cast<LONG>(height);

	mDepthD16 = new USHORT[mDepthWidth * mDepthHeight];
	mColorCoordinates = new LONG[mDepthWidth * mDepthHeight*2];
	mColorRGBX = new BYTE[mColorWidth * mColorHeight * 4];//*4 bytes

	mDepthReceived = false;
	mColorReceived = false;

	mxyScale = 0.0f;
}

KinectRenderer::~KinectRenderer()
{
	if (mpNuiSensor != NULL)
	{
		mpNuiSensor->NuiShutdown();
		mpNuiSensor->Release();
		std::cout << "Kinect shut down.\n";
	}

	ReleaseCOM(mpVertexBuffer);
	ReleaseCOM(mpKinectConstantBuffer);

	SAFE_DELETE(mpColorTexture);
	SAFE_DELETE(mpDepthTexture);

	CloseHandle(mhNextDepthFrameEvent);
	CloseHandle(mhNextColorFrameEvent);

	delete[] mDepthD16;
	delete[] mColorCoordinates;
	delete[] mColorRGBX;
}

void KinectRenderer::Initialize()
{
	//Create shader
	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_GEOMETRY, "GS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R16_SINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	mpKinectRenderShader = gpApplication->getRenderer()->loadShader(L"Shaders/kinect.hlsl", shaderInfo, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, vertexDescription, ARRAYSIZE(vertexDescription)); 

	//Create empty vertex buffer
	D3D11_BUFFER_DESC bd = {0};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(short);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

	HRESULT hr = gpApplication->getRenderer()->device()->CreateBuffer(&bd, NULL, &mpVertexBuffer);
    if ( FAILED(hr) ) { return; }

	//Create the kinect info constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBKinectInfo);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
	hr = gpApplication->getRenderer()->device()->CreateBuffer(&bd, NULL, &mpKinectConstantBuffer);
    if ( FAILED(hr) ) { return; }

	//Initialize Kinect
	int iSensorCount = 0;
	hr = NuiGetSensorCount(&iSensorCount);

	if (iSensorCount == 0 || FAILED(hr))
		MessageBox(NULL, L"No Kinect detected", L"Alart :p", MB_OK);

	hr = NuiCreateSensorByIndex(0, &mpNuiSensor);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create sensor", L"Alert", MB_OK);

	hr = mpNuiSensor->NuiStatus();
	if (hr != S_OK)
		MessageBox(NULL, L"Sensor not okay!", L"Alert", MB_OK);


	hr = mpNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR);
	if (FAILED(hr))
		MessageBox(NULL, L"Sensor not okay!", L"Alert", MB_OK);
	else
		std::cout << "Kinect initialized.\n";
	
	mhNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	hr = mpNuiSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
		NUI_DEPTH_RESOLUTION,
		0,
		2,
		mhNextDepthFrameEvent,
		&mpDepthStreamHandle);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create depth event handle", L"Alert", MB_OK);
	else
		std::cout << "Kinect depth stream opened.\n";

	mhNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	hr = mpNuiSensor->NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_COLOR_RESOLUTION,
        0,
        2,
        mhNextColorFrameEvent,
        &mpColorStreamHandle );
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create color event handle", L"Alert", MB_OK);
	else
		std::cout << "Kinect color stream opened.\n";


	// Calculate correct XY scaling factor so that our vertices are correctly placed in the world
    // This helps us to unproject from the Kinect's depth camera back to a 3d world
    // Since the Horizontal and Vertical FOVs are proportional with the sensor's resolution along those axes
    // We only need to do this for horizontal
    // I.e. tan(horizontalFOV)/depthWidth == tan(verticalFOV)/depthHeight
    // Essentially we're computing the vector that light comes in on for a given pixel on the depth camera
    // We can then scale our x&y depth position by this and the depth to get how far along that vector we are
    const float DegreesToRadians = 3.14159265359f / 180.0f;
	mxyScale = tanf(NUI_CAMERA_DEPTH_NOMINAL_HORIZONTAL_FOV * DegreesToRadians * 0.5f) / (mDepthWidth * 0.5f);    

	//Initialize textures
	mpColorTexture = gpApplication->getRenderer()->createTexture(Texture_RGBA | Texture_KinectDynamic, mColorWidth, mColorHeight);
	mpDepthTexture = gpApplication->getRenderer()->createTexture(Texture_Depth | Texture_KinectDynamic, mDepthWidth, mDepthHeight);
}

HRESULT KinectRenderer::ProcessDepth()
{
	D3DRenderer* renderer = gpApplication->getRenderer();
    NUI_IMAGE_FRAME imageFrame;

    HRESULT hr = mpNuiSensor->NuiImageStreamGetNextFrame(mpDepthStreamHandle, 0, &imageFrame);
    if ( FAILED(hr) ) { return hr; }
   
    NUI_LOCKED_RECT LockedRect;
    hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
    if ( FAILED(hr) ) { return hr; }

	memcpy(mDepthD16, LockedRect.pBits, LockedRect.size);
	mDepthReceived = true;

    hr = imageFrame.pFrameTexture->UnlockRect(0);
    if ( FAILED(hr) ) { return hr; };

    hr = mpNuiSensor->NuiImageStreamReleaseFrame(mpDepthStreamHandle, &imageFrame);

	//Flip the depth stream
	/*for (int y = 0; y < mDepthHeight; y++)
	{
		for (int x = 0; x < mDepthWidth / 2; x++)
		{
			USHORT temp = mDepthD16[x + y * mDepthWidth];
			mDepthD16[x + y * mDepthWidth] = mDepthD16[(mDepthWidth - x) + y * mDepthWidth - 1];
			mDepthD16[(mDepthWidth - x) + y * mDepthWidth - 1] = temp;
		}
	}*/

    // copy to our d3d 11 depth texture
    D3D11_MAPPED_SUBRESOURCE msT;
	hr = renderer->context()->Map(mpDepthTexture->getD3DTexture(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
    if ( FAILED(hr) ) { return hr; }

	memcpy(msT.pData, mDepthD16, LockedRect.size);    
    renderer->context()->Unmap(mpDepthTexture->getD3DTexture(), NULL);

    return hr;
}

HRESULT KinectRenderer::ProcessColor()
{
    NUI_IMAGE_FRAME imageFrame;

	HRESULT hr = mpNuiSensor->NuiImageStreamGetNextFrame(mpColorStreamHandle, 0, &imageFrame);
    if ( FAILED(hr) ) { return hr; }
  
    NUI_LOCKED_RECT LockedRect;
    hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
    if ( FAILED(hr) ) { return hr; }

	memcpy(mColorRGBX, LockedRect.pBits, LockedRect.size);
	mColorReceived = true;

	/*for (int y = 0; y < mColorHeight; y++)
	{
		for (int x = 0; x < mColorWidth / 2; x++)
		{
			BYTE temp = mColorRGBX[x + y * mColorWidth];
			mColorRGBX[x + y * mColorWidth] = mColorRGBX[(mColorWidth - x) + y * mColorWidth];
			mColorRGBX[mColorWidth - x + y * mColorWidth] = temp;
		}
	}*/

    hr = imageFrame.pFrameTexture->UnlockRect(0);
    if ( FAILED(hr) ) { return hr; };

	hr = mpNuiSensor->NuiImageStreamReleaseFrame(mpColorStreamHandle, &imageFrame);
    
    return hr;
}

HRESULT KinectRenderer::MapColorToDepth()
{
    HRESULT hr;
	D3DRenderer* renderer = gpApplication->getRenderer();
	ID3D11DeviceContext* context = renderer->context();

	LONG colorToDepthDivisor = mColorWidth / mDepthWidth;

    // Get of x, y coordinates for color in depth space
    // This will allow us to later compensate for the differences in location, angle, etc between the depth and color cameras
	mpNuiSensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
		NUI_COLOR_RESOLUTION,
        NUI_DEPTH_RESOLUTION,
		mDepthWidth * mDepthHeight,
		mDepthD16,
        mDepthWidth * mDepthHeight * 2,
		mColorCoordinates
        );

    // copy to our d3d 11 color texture
    D3D11_MAPPED_SUBRESOURCE msT;
	hr = context->Map(mpColorTexture->getD3DTexture(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msT);
    if ( FAILED(hr) ) { return hr; }
    
    // loop over each row and column of the color
	for (LONG y = 0; y < mColorHeight - 1; ++y)
    {
        LONG* pDest = (LONG*)((BYTE*)msT.pData + msT.RowPitch * y);
		for (LONG x = 0; x < mColorWidth; ++x)
        {
            // calculate index into depth array
			//int depthIndex = (mDepthWidth - x / colorToDepthDivisor) + (y/colorToDepthDivisor + 1) * mDepthWidth - 1;//Flip color buffer
			int depthIndex = x / colorToDepthDivisor + y/colorToDepthDivisor * mDepthWidth;

            // retrieve the depth to color mapping for the current depth pixel
			LONG colorInDepthX = mColorCoordinates[depthIndex * 2];
            LONG colorInDepthY = mColorCoordinates[depthIndex * 2 + 1];

            // make sure the depth pixel maps to a valid point in color space
			if ( colorInDepthX >= 0 && colorInDepthX < mColorWidth && colorInDepthY >= 0 && colorInDepthY < mColorHeight )
            {
                // calculate index into color array
				LONG colorIndex = colorInDepthX + colorInDepthY * mColorWidth;
				//LONG colorIndex = (mColorWidth - colorInDepthX) + (colorInDepthY + 1) * mColorWidth;

                // set source for copy to the color pixel
				LONG* pSrc = (LONG *)mColorRGBX + colorIndex;
                *pDest = *pSrc;
            }
            else
            {
                *pDest = 0;
            }

            pDest++;
        }
    }

	context->Unmap(mpColorTexture->getD3DTexture(), NULL);

    return hr;
}

void KinectRenderer::Update(float dt)
{
	bool needToMapColorToDepth = false;

    if ( WAIT_OBJECT_0 == WaitForSingleObject(mhNextDepthFrameEvent, 0) )
    {
        // if we have received any valid new depth data we may need to draw
        if ( SUCCEEDED(ProcessDepth()) )
        {
            needToMapColorToDepth = true;
        }
    }

	if ( WAIT_OBJECT_0 == WaitForSingleObject(mhNextColorFrameEvent, 0) )
    {
        // if we have received any valid new color data we may need to draw
        if ( SUCCEEDED(ProcessColor()) )
        {
            needToMapColorToDepth = true;
        }
    }

    // If we have not yet received any data for either color or depth since we started up, we shouldn't draw
	if (!mDepthReceived || !mColorReceived)
    {
        needToMapColorToDepth = false;
    }

    if (needToMapColorToDepth)
    {
        MapColorToDepth();
    }
}

void KinectRenderer::Render(D3DRenderer* renderer)
{
	Shader* previousShader = renderer->getActiveShader();
	renderer->setShader(mpKinectRenderShader);
	renderer->setTextureResource(0, mpDepthTexture);
	renderer->setTextureResource(1, mpColorTexture);

	CBPerFrame perframe = *renderer->getPerFrameBuffer();
	perframe.Projection = XMMatrixTranspose(perframe.Projection);
	perframe.View = XMMatrixTranspose(perframe.View);

	renderer->setPerFrameBuffer(perframe);

	CBPerObject perObject;
	perObject.World = XMMatrixTranslation(0.0f, 1.0f, 0.0f) * mTransform.getTransform();
	perObject.WorldInvTranspose = XMMatrixTranspose(perObject.World);
	perObject.WorldViewProj = renderer->getPerFrameBuffer()->ViewProj;

	renderer->setPerObjectBuffer(perObject);
	
	CBKinectInfo buffer;
	buffer.XYScale = XMFLOAT4(mxyScale, -mxyScale, 0.f, 0.f);
	renderer->context()->UpdateSubresource(mpKinectConstantBuffer, 0, NULL, &buffer, 0, 0);
	renderer->setConstantBuffer(2, mpKinectConstantBuffer);

	UINT stride = 0;
    UINT offset = 0;
	renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);

	renderer->context()->Draw(mDepthWidth * mDepthHeight, 0);

	renderer->setShader(previousShader);
}