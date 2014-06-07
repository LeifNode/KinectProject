#include "HydraManager.h"
#include "D3DRenderer.h"


HydraManager::HydraManager()
{
}

HydraManager::~HydraManager()
{
	sixenseExit();
}

void controlManagerSetupCallback(sixenseUtils::ControllerManager::setup_step step)
{
	std::cout << "Received call" << std::endl;
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

}

void HydraManager::Render(D3DRenderer* renderer)
{
	//std::cout << "Render called" << std::endl;

	sixenseAllControllerData acd;

	int left_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1L );
	int right_index = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1R );

	sixenseSetActiveBase(0);

	sixenseGetAllNewestData(&acd);

	for (int controller = 0; controller < sixenseGetMaxControllers(); controller++)
	{
		if (sixenseIsControllerEnabled(controller))
		{
			//if (controller == 0)
				//std::cout << "Left controller: ";
			//else if (controller == 1)
				//std::cout << "Right controller: ";


		}
	}
}