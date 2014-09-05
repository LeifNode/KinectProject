#include "HydraRenderer.h"
#include "InputSystem.h"
#include "MeshRenderer.h"
#include "Geometry.h"
#include "d3dApp.h"

HydraRenderer::HydraRenderer()
	:mpPointerRenderer(NULL),
	mpRootRenderer(NULL)
{

}

HydraRenderer::~HydraRenderer()
{
	SAFE_DELETE(mpPointerRenderer);
	SAFE_DELETE(mpRootRenderer);
}

void HydraRenderer::Initialize()
{
	mpPointerRenderer = new MeshRenderer<Vertex>();
	mpRootRenderer = new MeshRenderer<Vertex>();
	
	Mesh rootMesh;
	GeometryGenerator::CreateSphere(0.02f, 30, 30, rootMesh);

	mpRootRenderer->Initialize(rootMesh.Vertices, rootMesh.Indices, gpApplication->getRenderer());

	Mesh pointerMesh;
	GeometryGenerator::CreateCylinder(0.02f, 0.0f, 0.12f, 30, 10, pointerMesh);
	//GeometryGenerator::CreateCylinder(0.02f, 0.02f, 0.5f, 30, 10, pointerMesh);

	mpPointerRenderer->Initialize(pointerMesh.Vertices, pointerMesh.Indices, gpApplication->getRenderer());
}

void HydraRenderer::Render(D3DRenderer* renderer)
{
	CBPerObject perObject;
	perObject.Material.HasDiffuseTex = false;
	perObject.Material.HasNormalTex = false;
	perObject.Material.HasSpecTex = false;
	perObject.Material.HasEmissiveTex = false;

	HydraManager* hydra = InputSystem::get()->getHydra();

	for (int controller = 0; controller < sixenseGetMaxControllers(); controller++)
	{
		if (sixenseIsControllerEnabled(controller))
		{
			XMVECTOR rotationQuat = hydra->getRotation(controller);
			XMVECTOR position = hydra->getPosition(controller);
			//XMVectorSetZ(position, -XMVectorGetZ(position));//Flip Z axis
			//position += XMVectorSet(0.0f, 0.9f, 0.0f, 0.0f);//Offset for table height

			perObject.World = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), XMConvertToRadians(90.0f))) *
							  XMMatrixTranslation(0.0f, 0.0f, 0.07f) *
							  //XMMatrixTranslation(0.0f, 0.0f, 0.25f) *
							  XMMatrixRotationQuaternion(rotationQuat) *
							  XMMatrixTranslationFromVector(position);
			perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
			perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

			renderer->setPerObjectBuffer(perObject);

			mpPointerRenderer->Render(renderer);

			//Render root trackers
			perObject.World = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), XMConvertToRadians(90.0f))) *
							  XMMatrixRotationQuaternion(rotationQuat) * 
							  XMMatrixTranslationFromVector(position);
			perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
			perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

			renderer->setPerObjectBuffer(perObject);

			mpRootRenderer->Render(renderer);
		}
	}
}