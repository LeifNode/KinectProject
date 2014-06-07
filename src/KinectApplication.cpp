
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
#include "Camera.h"
#include "HydraManager.h"

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
	mpMainShader(nullptr)
{
	mMainWndCaption = L"Test App";

	mpMeshRenderer = new MeshRenderer<Vertex>();
	ZeroMemory(&mPerFrameData, sizeof(CBPerFrame));

	mpCamera = new Camera(XMFLOAT3(0.0f, 0.0f, 0.0f));
	mpHydraManager = new HydraManager();
}

KinectApplication::~KinectApplication()
{
	unhookInputEvents();

	SAFE_DELETE(mpMeshRenderer);
	SAFE_DELETE(mpCamera);
	SAFE_DELETE(mpHydraManager);
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
	GeometryGenerator::CreateBox(10.0f, 10.0f, 10.0f, sphere);

	mpMeshRenderer->Initialize(sphere.Vertices, sphere.Indices, mpRenderer);

	mpHydraManager->Initialize();

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
	mpCamera->OnResize(mClientWidth, mClientHeight);

	mPerFrameData.Projection = mpCamera->getProj();
	mPerFrameData.ProjectionInv = XMMatrixInverse(NULL, mpCamera->getProj());
}

void KinectApplication::Update(float dt)
{
	mpHydraManager->Update(dt);
	mpCamera->Update(dt);

	XMMATRIX view = mpCamera->getView();

	mPerFrameData.View = view;
	mPerFrameData.ViewInv = XMMatrixInverse(NULL, view);
	mPerFrameData.ViewProj = view * mPerFrameData.Projection;
	mPerFrameData.ViewProjInv = XMMatrixInverse(NULL, mPerFrameData.ViewProj);

	mPerFrameData.EyePosition = mpCamera->getPosition();
	mPerFrameData.EyeDirection = mpCamera->getDirection();

	mpInputSystem->Update();
}

void KinectApplication::Draw()
{
	mpRenderer->preRender();
	mpRenderer->setShader(mpMainShader);
	mpRenderer->setPerFrameBuffer(mPerFrameData);

	mpHydraManager->Render(mpRenderer);

	CBPerObject perObject;

	perObject.World = XMMatrixIdentity();
	perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
	perObject.WorldViewProj = mPerFrameData.ViewProj;
	perObject.TextureTransform = XMMatrixIdentity();

	mpRenderer->setPerObjectBuffer(perObject);

	mpMeshRenderer->Render(mpRenderer);

	mpRenderer->postRender();
}

void KinectApplication::onKeyDown(IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KEY_ESC)
	{
		mRunning = false;
	}
}

void KinectApplication::onMouseDown(IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_MouseDown>(eventData);

	SetCapture(mhWnd);
	ShowCursor(FALSE);

	mpCamera->OnMouseDown(dataPtr->getButton());
}

void KinectApplication::onMouseUp(IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_MouseUp>(eventData);

	ReleaseCapture();
	ShowCursor(TRUE);

	mpCamera->OnMouseUp(dataPtr->getButton());
}

void KinectApplication::onMouseMove(IEventDataPtr eventData)
{
	auto dataPtr = EVENT_CAST(EventData_MouseMove, eventData);
	const MouseState* state = InputSystem::get()->getMouseState();

	mpCamera->OnMouseMove(state->getX(), state->getY());

	if( state->getLeft() )
	{
		POINT newPos = { mClientWidth / 2, mClientHeight / 2 };
	
		ClientToScreen(mhWnd, &newPos);
		//SetCursorPos(newPos.x, newPos.y);
	}
}