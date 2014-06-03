
#include "KinectApplication.h"
#include "MathHelper.h"
#include "D3DRenderer.h"
#include "Events.h"
#include <iostream>
#include <DirectXMath.h>
#include "..\3rdParty\include\FastDelegate\FastDelegate.h"
#include "TextureManager.h"
#include "Geometry.h"
#include "MeshRenderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	UNREFERENCED_PARAMETER( prevInstance );
    UNREFERENCED_PARAMETER( cmdLine );

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	KinectApplication* application = new KinectApplication(hInstance);

	if (!application->Initialize())
		return 0;

	int returnVal = application->Run();

	delete application;

	return returnVal;
}

KinectApplication::KinectApplication(HINSTANCE hInstance)
	:
	D3DApp(hInstance),
	mpMainShader(nullptr),
	mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(150.0f), currentRot(0.0f)
{
	mMainWndCaption = L"Test App";

	mpMeshRenderer = new MeshRenderer<Vertex>();
	ZeroMemory(&mPerFrameData, sizeof(CBPerFrame));

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}

KinectApplication::~KinectApplication()
{
	unhookInputEvents();
}

bool KinectApplication::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	onResize();

	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	mpMainShader = mpRenderer->loadShader(L"Shaders/color.fx", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription)); 

	hookInputEvents();

	Mesh sphere;
	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, sphere);

	mpMeshRenderer->Initialize(sphere.Vertices, sphere.Indices, mpRenderer);

	return true;
}

void KinectApplication::hookInputEvents()
{
	EventSystem::get()->addListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onKeyDown));
	EventSystem::get()->addListener(EventData_MouseDown::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseDown));
	EventSystem::get()->addListener(EventData_MouseUp::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseUp));
	EventSystem::get()->addListener(EventData_MouseMove::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseMove));
}

void KinectApplication::unhookInputEvents()
{
	EventSystem::get()->removeListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onKeyDown));
	EventSystem::get()->removeListener(EventData_MouseDown::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseDown));
	EventSystem::get()->removeListener(EventData_MouseUp::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseUp));
	EventSystem::get()->removeListener(EventData_MouseMove::skEventType, fastdelegate::MakeDelegate(this, &KinectApplication::onMouseMove));
}

void KinectApplication::onResize()
{
	D3DApp::onResize();
	
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, aspectRatio(), 1.0f, 10000.0f);

	mPerFrameData.Projection = P;
}

void KinectApplication::Update(float dt)
{
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);

	XMStoreFloat3(&mPerFrameData.EyePosition, pos);
	XMVECTOR direction = XMVectorMultiply(pos, XMLoadFloat3(&XMFLOAT3(-1.0f, -1.0f, -1.0f)));
	XMStoreFloat3(&mPerFrameData.EyeDirection, XMVector3Normalize(direction));


	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

	mPerFrameData.View = view;

	mpInputSystem->Update();
}

void KinectApplication::Draw()
{
	mpRenderer->preRender();
	mpRenderer->setShader(mpMainShader);
	mpRenderer->setPerFrameBuffer(mPerFrameData);

	CBPerObject perObject;

	perObject.World = XMMatrixIdentity();
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = mPerFrameData.View * mPerFrameData.Projection;
	perObject.TextureTransform = XMMatrixIdentity();

	mpRenderer->setPerObjectBuffer(perObject);

	mpMeshRenderer->Render(mpRenderer);

	mpRenderer->postRender();
}

void KinectApplication::onKeyDown(IEventDataPtr eventData)
{
	shared_ptr<EventData_KeyboardDown> dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KEY_ESC)
	{
		mRunning = false;
	}
}

void KinectApplication::onMouseDown(IEventDataPtr eventData)
{
	shared_ptr<EventData_MouseDown> dataPtr = static_pointer_cast<EventData_MouseDown>(eventData);

	mLastMousePos.x = dataPtr->getX();
	mLastMousePos.y = dataPtr->getY();

	SetCapture(mhWnd);
	ShowCursor(FALSE);
}

void KinectApplication::onMouseUp(IEventDataPtr eventData)
{
	ReleaseCapture();
	ShowCursor(TRUE);
}

void KinectApplication::onMouseMove(IEventDataPtr eventData)
{
	const MouseState* state = InputSystem::get()->getMouseState();

	if( state->getLeft() )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(state->getX() - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(state->getY() - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( state->getRight() )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(state->getX() - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(state->getY() - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - 20.0f * dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 300.0f);
	}

	mLastMousePos.x = state->getX();
	mLastMousePos.y = state->getY();
}