#include "LeapManager.h"

using namespace Leap;
using namespace std;

LeapManager::LeapManager()
{
	mController.addListener(*this);
}

LeapManager::~LeapManager()
{
}

void LeapManager::onInit(const Controller& controller)
{
}

void LeapManager::DeInit()
{
	mController.removeListener(*this);
}

void LeapManager::onConnect(const Controller& controller)
{
	cout << "LEAP connected" << endl;
	/*controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);*/
	//controller.setPolicyFlags(Controller::POLICY_IMAGES);
	Controller::PolicyFlag addImagePolicy =
		(Controller::PolicyFlag)(Controller::PolicyFlag::POLICY_IMAGES | controller.policyFlags());

	controller.setPolicyFlags(addImagePolicy);
}

void LeapManager::onDisconnect(const Controller& controller)
{
	cout << "LEAP disconnected" << endl;
}
void LeapManager::onExit(const Controller& controller)
{
}

void LeapManager::onFrame(const Controller& controller)
{
}

void LeapManager::onFocusGained(const Controller& controller)
{
}

void LeapManager::onFocusLost(const Controller& controller)
{
}

void LeapManager::onDeviceChange(const Leap::Controller& controller)
{

}

void LeapManager::onServiceConnect(const Leap::Controller& controller)
{

}

void LeapManager::onServiceDisconnect(const Leap::Controller& controller)
{

}

void LeapManager::Initialize()
{
	
}

void LeapManager::Update(float dt)
{
	if (mController.frame().id() != mFrame.id())
	{
		mFrame = mController.frame();
		//cout << "New Frame: " << mFrame.id() << endl;
	}

	Frame frame = mController.frame();

	ImageList images = frame.images();
	for (int i = 0; i < 2; i++)
	{
		Image image = images[i];


	}
}

void LeapManager::setRotation(const XMVECTOR& rotationQuat)
{

}

void LeapManager::setViewTransform(const XMMATRIX& mat)
{
	mInverseTransform = XMMatrixInverse(NULL, mat);
}

XMVECTOR LeapManager::transformPosition(const Leap::Vector& input)
{
	XMVECTOR rotationQuat = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(-90.0f));

	return XMVector3Transform(XMVector3Rotate(XMLoadFloat3(&XMFLOAT3(-input.x / 1000.0f, (input.y + 60.0f) / 1000.0f, -input.z / 1000.0f)), rotationQuat), mInverseTransform);
}

XMVECTOR LeapManager::transformRotation(const Leap::Vector& input)
{
	return XMLoadFloat3(&XMFLOAT3(input.x, input.y, input.z));
}