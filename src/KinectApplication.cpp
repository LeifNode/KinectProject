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
#include "OVRRenderer.h"
#include "KinectRenderer.h"
#include "HydraRenderer.h"
#include "FontManager.h"
#include "TextRenderer.h"
#include "LineRenderer.h"
#include "LeapRenderer.h"
#include "LeapManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
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

	mpPlaneRenderer = new MeshRenderer<Vertex>();
	mpCubeRenderer = new MeshRenderer<Vertex>();
	ZeroMemory(&mPerFrameData, sizeof(CBPerFrame));

	mpOVRRenderer = new OVRRenderer();
	mpKinectRenderer = new KinectRenderer();

	mpCamera = new Camera(XMFLOAT3(0.0f, 1.66f, -2.0f));
	mpHydraRenderer = new HydraRenderer();
	mpText = new TextRenderer(100);
	mpLineRenderer = new LineRenderer();
	mpLeapRenderer = new LeapRenderer();
}

KinectApplication::~KinectApplication()
{
	unhookInputEvents();

	SAFE_DELETE(mpPlaneRenderer);
	SAFE_DELETE(mpCubeRenderer);
	SAFE_DELETE(mpCamera);
	SAFE_DELETE(mpHydraRenderer);
	SAFE_DELETE(mpOVRRenderer);
	SAFE_DELETE(mpKinectRenderer);
	SAFE_DELETE(mpText);
	SAFE_DELETE(mpLineRenderer);
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

	mpOVRRenderer->Initialize();

	mpKinectRenderer->Initialize();

	hookInputEvents();

	Mesh mesh;
	GeometryGenerator::CreateGrid(10.0f, 10.0f, 10, 10, mesh);

	mpPlaneRenderer->Initialize(mesh.Vertices, mesh.Indices, mpRenderer);

	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, mesh);

	mpCubeRenderer->Initialize(mesh.Vertices, mesh.Indices, mpRenderer);

	//mRotationTool.setTargetTransform(&mpKinectRenderer->mTransform);
	//mRotationTool.setTargetTransform(&mCubeRotation);
	//mRotationTool.setTargetTransform(&mpText->mTransform);

	mpHydraRenderer->Initialize();

	GameTimer timer;
	timer.Reset();
	timer.Start();

	Font* font = mpFontManager->loadFont("arial", "Fonts/arial.ttf", "Fonts/arialbd.ttf", "Fonts/ariali.ttf", "Fonts/arialbi.ttf");
	font->loadGlyphs(40, FONT_TYPE_NORMAL);
	font->loadGlyphs(40, FONT_TYPE_BOLD);
	font->loadGlyphs(40, FONT_TYPE_ITALIC);
	//std::cout << font->getUsedSpace() << std::endl;

	timer.Tick();
	std::cout << "Total time: " << timer.DeltaTime() << "s\n";

	mpText->Initialize();
	mpText->setFont(font);
	mpText->setTextSize(40);

	mpText->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer elementum lacus ac velit vehicula, a sodales tellus molestie. Vestibulum aliquet, nunc eu gravida bibendum, felis neque porttitor sapien, at aliquam dolor leo sit amet dolor. Morbi at risus eget magna tristique convallis sit amet at elit. Aenean in felis pellentesque, varius arcu et, rhoncus elit. Quisque dapibus sem quis lectus sodales sagittis. Morbi at feugiat libero. Sed sagittis lacus nec mauris luctus, vitae semper tellus accumsan. Nullam ac lobortis odio. Cras ac sem cursus, consectetur orci sit amet, facilisis ligula. Integer nec lorem sit amet nulla faucibus luctus. Fusce neque neque, molestie in quam non, vulputate faucibus urna. Vivamus cursus aliquam pharetra. Suspendisse lorem sapien, adipiscing dictum velit nec, dictum sodales justo. Etiam aliquam rhoncus arcu, vitae vehicula arcu molestie vitae. Suspendisse ultrices placerat nunc, at pulvinar urna lacinia ut. Suspendisse convallis eu magna sit amet pellentesque."
		"Quisque dapibus cursus tellus ac porttitor. Quisque id neque purus. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras condimentum, nunc vel facilisis varius, lorem nibh tincidunt ipsum, et semper augue nulla in erat. Nunc mattis erat justo, sodales tincidunt massa commodo a. Nullam massa felis, tempor eu erat non, hendrerit convallis massa. Vivamus nec enim pulvinar, interdum ante convallis, cursus est. Pellentesque ultricies bibendum faucibus. Morbi ultrices nulla diam, id bibendum odio iaculis nec. Vivamus placerat dui vel ante congue, sit amet bibendum elit imperdiet. Praesent justo diam, elementum in lacinia sit amet, posuere eget lacus."
"In urna sem, viverra in tortor quis, condimentum sollicitudin erat. Phasellus faucibus ullamcorper nisl eu convallis. Nam porttitor sapien ut elit feugiat volutpat. Morbi aliquam adipiscing metus a tempor. Interdum et malesuada fames ac ante ipsum primis in faucibus. Ut rhoncus vulputate felis in consequat. Mauris fringilla est eget vulputate ornare. Integer venenatis molestie malesuada. Morbi lorem nisl, malesuada a mauris vel, ultrices sagittis dui. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas et pellentesque dui. Vestibulum nec ullamcorper tortor. Sed accumsan justo eget eros pellentesque, fermentum ullamcorper odio malesuada. Duis cursus tellus non mi pellentesque lobortis."
"Vestibulum eu lectus ac diam tincidunt elementum a eu eros. Vestibulum tincidunt facilisis hendrerit. Sed vel tempus nibh. Quisque semper erat sed risus tincidunt, ut bibendum elit ultrices. Vestibulum at sapien nulla. Quisque neque quam, sollicitudin cursus sagittis in, volutpat sed lorem. Fusce ultrices nulla non est placerat, a gravida ante gravida. Vivamus ligula urna, dignissim sit amet erat sit amet, molestie porta mauris. Vivamus pellentesque lectus a metus lobortis bibendum. Proin faucibus urna at justo tristique porttitor."
"Vestibulum non faucibus risus, sed tempor nibh. Duis volutpat congue nibh, a condimentum nulla. Pellentesque rutrum eros quis facilisis vulputate. Donec mollis malesuada posuere. Aenean aliquet, eros sed ultrices malesuada, neque justo luctus magna, sit amet condimentum ante eros in urna. Aliquam vulputate leo sed lobortis feugiat. Donec nisl odio, aliquam eu lacinia a, mattis in leo. Curabitur at dui non diam scelerisque consequat id tincidunt urna. Fusce convallis nec eros at pretium. Proin non orci pulvinar, blandit felis id, sollicitudin est. In placerat imperdiet dignissim. Vivamus tristique nunc risus, eu congue ipsum consequat quis."
"Nullam volutpat odio mauris, ac semper nibh blandit eget. Curabitur aliquet lobortis sem ut sollicitudin. Nunc non odio lorem. Cras tempus facilisis dolor, eu tincidunt arcu laoreet sit amet. Mauris ligula justo, scelerisque varius dui eu, convallis suscipit purus. Morbi dolor risus, lacinia sed malesuada in, sodales ac velit. Duis a ante a lorem eleifend suscipit eu vel sapien. Nullam tempor porttitor diam, et convallis eros eleifend id. Nam ligula felis, bibendum nec suscipit id, viverra ut justo. Nulla dictum facilisis leo, sed malesuada justo. Maecenas ut nibh varius, facilisis nulla in, lobortis nisl."
"Maecenas consequat tincidunt nisi malesuada ornare. Nulla at metus convallis, vulputate ipsum eu, sodales mi. In ornare nec tellus non pellentesque. Aenean ac pharetra felis. Suspendisse fringilla iaculis mollis. Aenean eros erat, cursus id molestie fringilla, scelerisque ut ipsum. In quis sagittis ipsum. Suspendisse potenti. Sed vel ante odio. Sed eleifend vestibulum velit quis tincidunt."
"Quisque dapibus cursus tellus ac porttitor. Quisque id neque purus. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras condimentum, nunc vel facilisis varius, lorem nibh tincidunt ipsum, et semper augue nulla in erat. Nunc mattis erat justo, sodales tincidunt massa commodo a. Nullam massa felis, tempor eu erat non, hendrerit convallis massa. Vivamus nec enim pulvinar, interdum ante convallis, cursus est. Pellentesque ultricies bibendum faucibus. Morbi ultrices nulla diam, id bibendum odio iaculis nec. Vivamus placerat dui vel ante congue, sit amet bibendum elit imperdiet. Praesent justo diam, elementum in lacinia sit amet, posuere eget lacus."
"In urna sem, viverra in tortor quis, condimentum sollicitudin erat. Phasellus faucibus ullamcorper nisl eu convallis. Nam porttitor sapien ut elit feugiat volutpat. Morbi aliquam adipiscing metus a tempor. Interdum et malesuada fames ac ante ipsum primis in faucibus. Ut rhoncus vulputate felis in consequat. Mauris fringilla est eget vulputate ornare. Integer venenatis molestie malesuada. Morbi lorem nisl, malesuada a mauris vel, ultrices sagittis dui. Interdum et malesuada fames ac ante ipsum primis in faucibus. Maecenas et pellentesque dui. Vestibulum nec ullamcorper tortor. Sed accumsan justo eget eros pellentesque, fermentum ullamcorper odio malesuada. Duis cursus tellus non mi pellentesque lobortis.");
	
	mpLeapRenderer->Initialize();

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
	//delete mpOVRRenderer;
	D3DApp::onResize();
	mpCamera->OnResize(mClientWidth, mClientHeight);

	mpOVRRenderer->OnResize();
	
	//mpOVRRenderer = new OVRRenderer();
	//mpOVRRenderer->Initialize();

	mPerFrameData.Projection = mpCamera->getProj();
	mPerFrameData.ProjectionInv = XMMatrixInverse(NULL, mpCamera->getProj());
}

void KinectApplication::Update(float dt)
{
	mpCamera->Update(dt);

	mpKinectRenderer->Update(dt);

	mpOVRRenderer->Update(dt);

	mRotationTool.Update(dt);

	mpInputSystem->Update(dt);

	/*mpLineRenderer->Points.clear();
	mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(0));
	mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
	mpLineRenderer->reloadPoints();
*/
	/*static int pointCount = 0;

	if (mpInputSystem->getHydra()->getTrigger(1) > 0.8f)
	{
		pointCount++;

		if (pointCount % 20 == 0)
		{
			if (mpLineRenderer->Points.List.size() > 1)
				mpLineRenderer->Points.addPoint(mpLineRenderer->Points.List[mpLineRenderer->Points.List.size() - 1]);

			mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
			mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
			mpLineRenderer->reloadPoints();
		}
	}*/

	mpLeapRenderer->Update();
}

void KinectApplication::Draw()
{
	mpRenderer->preRender();
	mpRenderer->setShader(mpMainShader);
	mpRenderer->setPerFrameBuffer(mPerFrameData);
	
#if USE_RIFT
	for (int i = 0; i < 2; i++)
	{
		mpOVRRenderer->PreRender(i);

		//Per frame buffer update

		//Convert quaterinon to correct axis
		XMVECTOR rotQuat = XMLoadFloat4(&XMFLOAT4(mpOVRRenderer->mEyeRenderPoses[i].Orientation.x,
												  mpOVRRenderer->mEyeRenderPoses[i].Orientation.y,
												  mpOVRRenderer->mEyeRenderPoses[i].Orientation.z,
												  mpOVRRenderer->mEyeRenderPoses[i].Orientation.w));
		
		XMVECTOR axis;
		float angle;

		XMQuaternionToAxisAngle(&axis, &angle, rotQuat);

		axis = XMVectorSet(XMVectorGetX(axis), XMVectorGetY(axis), -XMVectorGetZ(axis), 0.0f);

		rotQuat = XMQuaternionRotationAxis(axis, -angle);

		XMVECTOR offset = XMVectorSet(mpOVRRenderer->mEyeRenderPoses[i].Position.x,
									  mpOVRRenderer->mEyeRenderPoses[i].Position.y,
									  mpOVRRenderer->mEyeRenderPoses[i].Position.z,
									  0.0f);

		XMMATRIX view = mpCamera->getView(offset, rotQuat);

		mPerFrameData.Projection = mpOVRRenderer->getProjection(i);
		mPerFrameData.ProjectionInv = XMMatrixInverse(NULL, mPerFrameData.Projection);
		XMStoreFloat3(&mPerFrameData.EyePosition, XMLoadFloat3(&mpCamera->position) + offset);
#else
		XMMATRIX view = mpCamera->getView(XMVectorZero(), XMQuaternionIdentity());
		mPerFrameData.EyePosition = mpCamera->getPosition();
#endif
		
		mPerFrameData.View = view;
		mPerFrameData.ViewInv = XMMatrixInverse(NULL, view);
		mPerFrameData.ViewProj = view * mPerFrameData.Projection;
		mPerFrameData.ViewProjInv = XMMatrixInverse(NULL, mPerFrameData.ViewProj);

		mPerFrameData.EyeDirection = mpCamera->getDirection();

		mpRenderer->setPerFrameBuffer(mPerFrameData);

		LeapManager::getInstance().setViewTransform(mPerFrameData.View);
		
		//Per object for the plane mesh
		CBPerObject perObject;

		perObject.World = XMMatrixIdentity();
		perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
		perObject.WorldViewProj = mpRenderer->getPerFrameBuffer()->ViewProj;
		perObject.TextureTransform = XMMatrixIdentity();

		mpRenderer->setPerObjectBuffer(perObject);

		mpPlaneRenderer->Render(mpRenderer);

		//Render cube
		perObject.World = mCubeRotation.getTransform();
		perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
		perObject.WorldViewProj = perObject.World * mpRenderer->getPerFrameBuffer()->ViewProj;
		perObject.TextureTransform = XMMatrixIdentity();

		mpRenderer->setPerObjectBuffer(perObject);

		//mpCubeRenderer->Render(mpRenderer);

		mpHydraRenderer->Render(mpRenderer);

		//mpKinectRenderer->Render(mpRenderer);

		mpLeapRenderer->Render(mpRenderer);

		mpFontManager->bindRender(mpRenderer);

		std::stringstream stream;
		stream << std::string(mMainWndCaptionFull.begin(), mMainWndCaptionFull.end()) << " Points: " << mpLineRenderer->Points.List.size();

		mpText->setText(stream.str());
		mpText->Render(mpRenderer);

		mpLineRenderer->Render(mpRenderer);

		mpRenderer->setShader(mpMainShader);
		mpRenderer->setBlendState(false);
		
#if USE_RIFT
		mpOVRRenderer->PostRender(i);
	}
#endif

	mpRenderer->postRender();
	mpOVRRenderer->EndFrame();
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