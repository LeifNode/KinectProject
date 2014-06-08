#include "HydraManager.h"
#include "D3DRenderer.h"
#include "MeshRenderer.h"
#include "Geometry.h"
#include "d3dApp.h"

HydraManager::HydraManager()
	:mpPointerRenderer(NULL)
{
}

HydraManager::~HydraManager()
{
	sixenseExit();

	SAFE_DELETE(mpPointerRenderer);
	SAFE_DELETE(mpRootRenderer);
}

void controlManagerSetupCallback(sixenseUtils::ControllerManager::setup_step step)
{
	std::cout << sixenseUtils::getTheControllerManager()->getStepString() << std::endl;
}

void HydraManager::Initialize()
{
	sixenseInit();

	sixenseUtils::ControllerManager* controllerManager = sixenseUtils::getTheControllerManager();

	sixenseUtils::getTheControllerManager()->setGameType( sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER );
	sixenseUtils::getTheControllerManager()->registerSetupCallback(controlManagerSetupCallback);

	mpPointerRenderer = new MeshRenderer<Vertex>();
	mpRootRenderer = new MeshRenderer<Vertex>();
	
	Mesh rootMesh;
	GeometryGenerator::CreateSphere(0.02f, 30, 30, rootMesh);

	mpRootRenderer->Initialize(rootMesh.Vertices, rootMesh.Indices, gpApplication->getRenderer());

	Mesh pointerMesh;
	//GeometryGenerator::CreateCylinder(0.02f, 0.0f, 0.12f, 30, 10, pointerMesh);
	GeometryGenerator::CreateCylinder(0.02f, 0.02f, 0.5f, 30, 10, pointerMesh);

	mpPointerRenderer->Initialize(pointerMesh.Vertices, pointerMesh.Indices, gpApplication->getRenderer());
}

void HydraManager::Update(float dt)
{

}

void HydraManager::Render(D3DRenderer* renderer)
{
	sixenseAllControllerData acd;

	int left_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1L );
	int right_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1R );

	sixenseSetActiveBase(0);

	sixenseGetAllNewestData(&acd);

	CBPerObject perObject;

	for (int controller = 0; controller < sixenseGetMaxControllers(); controller++)
	{
		if (sixenseIsControllerEnabled(controller))
		{
			//if (controller == 0)
				//std::cout << "Left controller: ";
			//else if (controller == 1)
				//std::cout << "Right controller: ";

			XMVECTOR axis;
			float angle;

			XMQuaternionToAxisAngle(&axis, &angle, XMLoadFloat4(&XMFLOAT4(acd.controllers[controller].rot_quat)));

			axis = XMVectorSet(XMVectorGetX(axis), XMVectorGetY(axis), -XMVectorGetZ(axis), 0.0f);

			XMVECTOR rotationQuat = XMQuaternionRotationAxis(axis, -angle);

			perObject.World = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), XMConvertToRadians(90.0f))) *
							  //XMMatrixTranslation(0.0f, 0.0f, 0.07f) *
							  XMMatrixTranslation(0.0f, 0.0f, 0.25f) *
							  XMMatrixRotationQuaternion(rotationQuat) *
							  XMMatrixTranslation(acd.controllers[controller].pos[0] / 1000.0f, acd.controllers[controller].pos[1] / 1000.0f + 0.9f, -acd.controllers[controller].pos[2] / 1000.0f);
			perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
			perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

			renderer->setPerObjectBuffer(perObject);

			mpPointerRenderer->Render(renderer);

			//Render root trackers
			perObject.World = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), XMConvertToRadians(90.0f))) *
							  XMMatrixRotationQuaternion(rotationQuat) * 
							  XMMatrixTranslation(acd.controllers[controller].pos[0] / 1000.0f, acd.controllers[controller].pos[1] / 1000.0f + 0.9f, -acd.controllers[controller].pos[2] / 1000.0f);
			perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
			perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

			renderer->setPerObjectBuffer(perObject);

			mpRootRenderer->Render(renderer);
		}
	}
}