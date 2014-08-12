#include "HydraManager.h"
#include "D3DRenderer.h"
#include "MeshRenderer.h"
#include "Geometry.h"
#include "d3dApp.h"

HydraManager::HydraManager()
	:mPointerDistance(0.13f)
{
}

HydraManager::~HydraManager()
{
	sixenseExit();
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
}

void HydraManager::Update(float dt)
{
	int left_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1L );
	int right_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1R );

	sixenseSetActiveBase(0);

	sixenseGetAllNewestData(&mAcd);
}

XMVECTOR HydraManager::getPosition(int controllerIndex) const
{
	if (sixenseIsControllerEnabled(controllerIndex))
		return XMLoadFloat3(&XMFLOAT3(-mAcd.controllers[controllerIndex].pos[0], 
									  mAcd.controllers[controllerIndex].pos[1] + 900.0f, //Table height offset in mm
									  -mAcd.controllers[controllerIndex].pos[2])) / 1000.0f;
	
	return XMVectorZero();
}

XMVECTOR HydraManager::getRotation(int controllerIndex) const
{
	if (sixenseIsControllerEnabled(controllerIndex))
	{
		XMVECTOR axis;
		float angle;

		XMQuaternionToAxisAngle(&axis, &angle, XMLoadFloat4(&XMFLOAT4(&mAcd.controllers[controllerIndex].rot_quat[0])));

		axis = XMVectorSet(-XMVectorGetX(axis), XMVectorGetY(axis), -XMVectorGetZ(axis), 0.0f);

		XMVECTOR rotationQuat = XMQuaternionRotationAxis(axis, angle);

		return rotationQuat;
	}
	
	return XMQuaternionIdentity();
}

XMVECTOR HydraManager::getPointerPosition(int controllerIndex) const
{
	XMVECTOR position = getPosition(controllerIndex);

	return position + XMVector3Rotate(XMVectorSet(0.0f, 0.0f, mPointerDistance, 0.0f), getRotation(controllerIndex)); 
}

unsigned int HydraManager::getButtons(int controllerIndex) const
{ 
	if (controllerIndex < 2 && sixenseIsControllerEnabled(controllerIndex)) 
		return mAcd.controllers[controllerIndex].buttons; 
	else 
		return 0; 
}

float HydraManager::getTrigger(int controllerIndex) const
{
	if (sixenseIsControllerEnabled(controllerIndex))
		return mAcd.controllers[controllerIndex].trigger;
	else 
		return 0.0f;
}

XMFLOAT2 HydraManager::getJoystick(int controllerIndex) const
{
	XMFLOAT2 directionVector(0.0f, 0.0f);

	if (sixenseIsControllerEnabled(controllerIndex))
	{
		XMStoreFloat2(&directionVector, XMVector2Normalize(XMVectorSet(mAcd.controllers[controllerIndex].joystick_x, 
																	   mAcd.controllers[controllerIndex].joystick_y,
																	   0.0f, 0.0f)));
	}

	return directionVector;
}

bool HydraManager::isDocked(int controllerIndex) const
{
	if (sixenseIsControllerEnabled(controllerIndex) && mAcd.controllers[controllerIndex].is_docked != 1)
		return false;

	return true;
}