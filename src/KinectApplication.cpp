#include "KinectApplication.h"
#include "MathHelper.h"
#include "D3DRenderer.h"
#include "Events.h"
#include <iostream>
#include <DirectXMath.h>
#include "TextureManager.h"
#include "Geometry.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "KinectRenderer.h"
#include "HydraRenderer.h"
#include "FontManager.h"
#include "TextRenderer.h"
#include "LineRenderer.h"
#include "LeapRenderer.h"
#include "LeapManager.h"
#include "PhysicsSystem.h"
#include "COLLADALoader.h"
#include "ParticleSystem.h"
#include "PaintingSystem.h"
#include "FractalRenderer.h"

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
	//mpCubeRenderer = new MeshRenderer<Vertex>();
	ZeroMemory(&mPerFrameData, sizeof(CBPerFrame));

	mpKinectRenderer = new KinectRenderer();

	mpCamera = new Camera(XMFLOAT3(0.0f, 1.6f, 0.0f));
	//mpHydraRenderer = new HydraRenderer();
	mpText = new TextRenderer(100);
	//mpLineRenderer = new LineRenderer();
	mpLeapRenderer = new LeapRenderer();
	mpParticleSystem = new ParticleSystem();

	mpPaintingSystem = new PaintingSystem();

	mpFractalRenderer = new FractalRenderer();

	mSimluateParticles = true;
	mRenderSponza = true;
	mParticleSimulationSpeed = 1.0f;
}

KinectApplication::~KinectApplication()
{
	unhookInputEvents();

	SAFE_DELETE(mpPlaneRenderer);

	for (int i = 0; i < mpCubeRendererArr.size(); i++)
		delete mpCubeRendererArr[i];
	for (auto it = mCubeMaterials.begin(); it != mCubeMaterials.end(); ++it)
		delete it->second;
	//SAFE_DELETE(mpCubeRenderer);
	SAFE_DELETE(mpCamera);
	//SAFE_DELETE(mpHydraRenderer);
	SAFE_DELETE(mpKinectRenderer);
	SAFE_DELETE(mpText);
	//SAFE_DELETE(mpLineRenderer);
	SAFE_DELETE(mpParticleSystem);
	SAFE_DELETE(mpLeapRenderer);

	SAFE_DELETE(mpPaintingSystem);
}

bool KinectApplication::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	onResize();

	Shader::ShaderInfo shaderInfo[] = {
		{ Shader::SHADER_TYPE_VERTEX, "VS" },
		{ Shader::SHADER_TYPE_PIXEL, "PS" },
		{ Shader::SHADER_TYPE_NONE, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	mpMainShader = mpRenderer->loadShader(L"Shaders/color.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription)); 

	//mpKinectRenderer->Initialize();

	hookInputEvents();

	Mesh mesh;
	GeometryGenerator::CreateGrid(10.0f, 10.0f, 10, 10, mesh);

	mpPlaneRenderer->Initialize(mesh.Vertices, mesh.Indices, mpRenderer);

	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, mesh);

	//mpCubeRenderer->Initialize(mesh.Vertices, mesh.Indices, mpRenderer);
	//COLLADALoader loader("bunny_3ds.dae");
	COLLADALoader loader("sponza2.dae");
	//COLLADALoader loader("car.dae");
	loader.loadDocument();
	loader.parse();

	//mpCubeRenderer->Initialize(loader.getRootNode()->children[100]->model->subMeshes[0]->mesh.Vertices, loader.getRootNode()->children[100]->model->subMeshes[0]->mesh.Indices, mpRenderer);

	loadModels(loader.getRootNode(), &loader);
	loadTextures(&loader);
	
	//mRotationTool.setTargetTransform(&mpKinectRenderer->mTransform);
	mRotationTool.setTargetTransform(&mCubeRotation);
	//mRotationTool.setTargetTransform(&mpText->mTransform);

	//mpHydraRenderer->Initialize();

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

	mpParticleSystem->Initialize();
	mpLeapRenderer->Initialize();

	//mpPaintingSystem->Initialize();

	mpFractalRenderer->Initialize();
	mpFractalRenderer->transform.setTranslation(XMVectorSet(0.f, 1.7f, 1.0f, 0.0f));
	mpFractalRenderer->transform.setRotation(XMVectorSet(0.0f, MathHelper::Pi, 0.0f, 0.0f));
	mpFractalRenderer->transform.setScale(0.7f);

	return true;
}

void KinectApplication::loadModels(collada::SceneNode* node, COLLADALoader* loader)
{
	int size = (int)node->children.size();
	for (int i = 0; i < size; i++)
	{
		if (node->children[i]->model != NULL)
		{
			MeshRenderer<Vertex>* meshRenderer = new MeshRenderer<Vertex>();

			meshRenderer->Initialize(node->children[i]->model->subMeshes[0]->mesh.Vertices, node->children[i]->model->subMeshes[0]->mesh.Indices, mpRenderer);

			mpCubeRendererArr.push_back(meshRenderer);

			if (node->children[i]->effectId != "")
			{
				const collada::Effect* effect = loader->getEffect(node->children[i]->effectId);

				assettypes::Material* mat = new assettypes::Material();

				if (effect->diffuseTextureId != "")
					mat->texturePaths.insert(std::make_pair(assettypes::TextureType_DIFFUSE, effect->diffuseTextureId));
				if (effect->bumpMapId != "")
					mat->texturePaths.insert(std::make_pair(assettypes::TextureType_NORMAL, effect->bumpMapId));
				if (effect->specularMapId != "")
					mat->texturePaths.insert(std::make_pair(assettypes::TextureType_SPECULAR, effect->specularMapId));
				if (effect->emissiveMapId != "")
					mat->texturePaths.insert(std::make_pair(assettypes::TextureType_EMISSIVE, effect->emissiveMapId));

				mCubeMaterials.insert(std::make_pair(meshRenderer, mat));
			}

			if (node->children[i]->children.size() > 0)
				loadModels(node->children[i], loader);
		}
	}
}

void KinectApplication::loadTextures(COLLADALoader* loader)
{
	for (auto it = loader->mImages.begin(); it != loader->mImages.end(); ++it)
	{
		Texture* tex = getTextureManager()->getTexture(it->second.filePath);
	}
}

void KinectApplication::bindTextures(MeshRenderer<Vertex>* mesh, Material& matOut)
{
	matOut.HasDiffuseTex = false;
	matOut.HasNormalTex = false;
	matOut.HasSpecTex = false;
	matOut.HasEmissiveTex = false;

	auto cubeMat = mCubeMaterials.find(mesh);
	if (cubeMat != mCubeMaterials.end())
	{
		for (auto it = cubeMat->second->texturePaths.begin(); it != cubeMat->second->texturePaths.end(); ++it)
		{
			Texture* texture = mpTextureManager->getTexture(it->second);

			if (texture != NULL)
			{
				switch(it->first)
				{
				case assettypes::TextureType_DIFFUSE:
					matOut.HasDiffuseTex = true;
					mpRenderer->setTextureResource(0, texture);
					break;
				case assettypes::TextureType_NORMAL:
					matOut.HasNormalTex = true;
					mpRenderer->setTextureResource(1, texture);
					break;
				case assettypes::TextureType_SPECULAR:
					matOut.HasSpecTex = true;
					mpRenderer->setTextureResource(2, texture);
					break;
				case assettypes::TextureType_EMISSIVE:
					matOut.HasEmissiveTex = true;
					//mpRenderer->setTextureResource(0, texture);
					break;
				default:
					break;
				}
			}
		}
	}
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

	//mpOVRRenderer->OnResize();

	mPerFrameData.Projection = mpCamera->getProj();
	mPerFrameData.ProjectionInv = XMMatrixInverse(NULL, mpCamera->getProj());
}

void KinectApplication::Update(float dt)
{
	mpCamera->Update(dt);

	//mpKinectRenderer->Update(dt);

	mRotationTool.Update(dt);

	mpInputSystem->Update(dt);

	//mpPaintingSystem->Update(dt);
	
	//mpPhysicsSystem->Update(dt);
	if (InputSystem::get()->getKeyboardState()->isKeyPressed(KEY_UP))
		mParticleSimulationSpeed = MathHelper::Clamp(mParticleSimulationSpeed + 0.3f * dt, 0.05f, 2.0f);
	if (InputSystem::get()->getKeyboardState()->isKeyPressed(KEY_DOWN))
		mParticleSimulationSpeed = MathHelper::Clamp(mParticleSimulationSpeed - 0.3f * dt, 0.05f, 2.0f);

	if (mSimluateParticles)
		mpParticleSystem->Update(dt * mParticleSimulationSpeed);

	/*mpLineRenderer->Points.clear();
	mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(0));
	mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
	mpLineRenderer->reloadPoints();
*/
	//updateLines(dt);

	mpLeapRenderer->Update();
}

void KinectApplication::updateLines(float dt)
{
	static int pointCount = 0;

	mpLineRenderer->setDrawMode(LineRenderer::LINE_DRAW_MODE_LINESTRIP);
	mpLineRenderer->setLineThickness(0.01f);

//	if (mpInputSystem->getHydra()->getTrigger(1) > 0.8f)
//	{
//		/*pointCount++;
//
//		if (pointCount % 20 == 0)
//		{
//			if (mpLineRenderer->Points.List.size() > 1)
//				mpLineRenderer->Points.addPoint(mpLineRenderer->Points.List[mpLineRenderer->Points.List.size() - 1]);
//*/
//			//mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
//			mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));
//			mpLineRenderer->reloadPoints();
//		//}
//	}

	static std::vector<XMFLOAT3> velocities;
	static float timeToDelete = 0.5f;

	std::vector<XMFLOAT3> tempVelocities;
	std::vector<XMFLOAT3> tempPoints;

	if (mpInputSystem->getHydra()->getTrigger(1) > 0.8f)
	{
		mpLineRenderer->Points.addPoint(mpInputSystem->getHydra()->getPointerPosition(1));

		XMVECTOR direction = XMVector3Normalize(mpInputSystem->getHydra()->getPointerPosition(1) - mpInputSystem->getHydra()->getPosition(1)) * 0.002f;
		XMFLOAT3 velocity;
		XMStoreFloat3(&velocity, direction);

		velocities.push_back(velocity);
	}

	for (int i = 0; i < mpLineRenderer->Points.List.size(); i++)
	{
		mpLineRenderer->Points.List[i].x = mpLineRenderer->Points.List[i].x + velocities[i].x;
		mpLineRenderer->Points.List[i].y = mpLineRenderer->Points.List[i].y + velocities[i].y;
		mpLineRenderer->Points.List[i].z = mpLineRenderer->Points.List[i].z + velocities[i].z;

		XMVECTOR newVel = XMLoadFloat3(&velocities[i]);
		newVel += newVel * dt * 0.01f;

		XMStoreFloat3(&velocities[i], newVel);
	}

	timeToDelete -= dt;

	if (timeToDelete <= 0.0f)
	{
		timeToDelete = 0.03f;

		for (int i = 2; i < mpLineRenderer->Points.List.size(); i++)
		{
			tempVelocities.push_back(velocities[i]);
			tempPoints.push_back(mpLineRenderer->Points.List[i]);
		}

		velocities = tempVelocities;
		mpLineRenderer->Points.List = tempPoints;
	}

	mpLineRenderer->reloadPoints();
}

void KinectApplication::Draw()
{
	mpRenderer->preRender();
	mpRenderer->setShader(mpMainShader);
	mpRenderer->setPerFrameBuffer(mPerFrameData);
	
	for (int i = 0; i < (mpRenderer->isUsingHMD() ? 2 : 1); i++)
	{
		XMVECTOR rotQuat = XMQuaternionIdentity();
		XMVECTOR offset;
		XMMATRIX view;
		XMVECTOR headPosition = XMVectorZero();

		if (mpRenderer->isUsingHMD())
		{
			mpRenderer->getHMD()->PreRender(i);

			headPosition = mpRenderer->getHMD()->getHeadPosition();

			//Per frame buffer update

			//Convert quaterinon to correct axis
			rotQuat = mpRenderer->getHMD()->getHeadOrientation();

			offset = mpRenderer->getHMD()->getEyeOffset(i);

			view = mpCamera->getView(offset, rotQuat, mpRenderer->getHMD()->getHeadPosition());

			mPerFrameData.Projection = mpRenderer->getHMD()->getProjection(i);
			mPerFrameData.ProjectionInv = XMMatrixInverse(NULL, mPerFrameData.Projection);
			mPerFrameData.EyePosition = mpCamera->getPosition();
			mPerFrameData.HeadPosition = mpCamera->getPosition();
			XMStoreFloat3(&mPerFrameData.EyePosition, XMLoadFloat3(&mpCamera->getPosition()) + mpRenderer->getHMD()->getHeadPosition() + XMVector3Rotate(offset, rotQuat));
			//XMStoreFloat3(&mPerFrameData.HeadPosition, XMLoadFloat3(&mpCamera->getPosition()) + mpRenderer->getHMD()->getHeadPosition());
			
			//XMStoreFloat3(&mPerFrameData.EyePosition, XMLoadFloat3(&mpCamera->position));
			
			LeapManager::getInstance().mEyeRelief = XMVectorGetZ(offset);
		}
		else
		{
			view = mpCamera->getView(XMVectorZero(), XMQuaternionIdentity(), XMVectorZero());
			mPerFrameData.EyePosition = mpCamera->getPosition();
			mPerFrameData.HeadPosition = mpCamera->getPosition();
		}

		LeapManager::getInstance().setViewTransform(mpCamera->getView(XMVectorZero(), rotQuat, headPosition));
		
		mPerFrameData.View = view;
		mPerFrameData.ViewInv = XMMatrixInverse(NULL, mPerFrameData.View);
		mPerFrameData.ViewProj = view * mPerFrameData.Projection;
		mPerFrameData.ViewProjInv = XMMatrixInverse(NULL, mPerFrameData.ViewProj);

		mPerFrameData.EyeDirection = mpCamera->getDirection();

		mpRenderer->setPerFrameBuffer(mPerFrameData);

		
		
		//Per object for the plane mesh
		CBPerObject perObject;
		Material objectMat = Material();
		objectMat.HasDiffuseTex = false;
		objectMat.HasNormalTex = false;
		objectMat.HasSpecTex = false;
		objectMat.HasEmissiveTex = false;

		perObject.Material = objectMat;
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

		if (mRenderSponza)
		{
			for (int j = 0; j < mpCubeRendererArr.size(); j++)
			{
				bindTextures(mpCubeRendererArr[j], objectMat);
				perObject.Material = objectMat;

				mpRenderer->setPerObjectBuffer(perObject);

				mpCubeRendererArr[j]->Render(mpRenderer);
			}
		}

		//mpCubeRenderer->Render(mpRenderer);

		//mpHydraRenderer->Render(mpRenderer);
		//mpPhysicsSystem->Render(mpRenderer);

		//mpKinectRenderer->Render(mpRenderer);

		mpLeapRenderer->Render(mpRenderer, i);

		//mpFractalRenderer->Render(mpRenderer);

		mpFontManager->bindRender(mpRenderer);

		std::stringstream stream;
		stream << std::string(mMainWndCaptionFull.begin(), mMainWndCaptionFull.end()) << " Simulation speed: " << mParticleSimulationSpeed; //<< " Points: " << mpLineRenderer->Points.List.size();

		mpText->setText(stream.str());
		mpText->Render(mpRenderer);

		//mpLineRenderer->Render(mpRenderer);
		
		//mpParticleSystem->Render(mpRenderer);

		//mpPaintingSystem->Render(mpRenderer);

		mpRenderer->setShader(mpMainShader);
		mpRenderer->setBlendState(false);
		mpRenderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
		mpRenderer->resetSamplerState();
	}

	//mpRenderer->renderDeferredLighting();

	mpRenderer->postRender();
}

void KinectApplication::onKeyDown(IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KEY_ESC)
	{
		mRunning = false;
	}
	else if (dataPtr->getKey() == KEY_F)
		mSimluateParticles = !mSimluateParticles;
	else if (dataPtr->getKey() == KEY_R)
		mRenderSponza = !mRenderSponza;
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