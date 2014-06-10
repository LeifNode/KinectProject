#include "TransformTool.h"

TransformTool::TransformTool()
	:mpTargetTransform(NULL)
{

}

TransformTool::~TransformTool()
{

}

void TransformTool::Update(float dt)
{
	HydraManager* hydra = InputSystem::get()->getHydra();

	mPreviousHydraPositions[0] = mHydraPositions[0];
	mPreviousHydraPositions[1] = mHydraPositions[1];

	mHydraPositions[0] = hydra->getPosition(0);
	mHydraPositions[1] = hydra->getPosition(1);

	if (mpTargetTransform != NULL && hydra->getTrigger(0) > 0.2f && hydra->getTrigger(1) > 0.2f)
	{
		XMVECTOR oldOrientationVector = XMVector3Normalize(mPreviousHydraPositions[0] - mPreviousHydraPositions[1]);
		XMVECTOR orientationVector = XMVector3Normalize(mHydraPositions[0] - mHydraPositions[1]);

		if (!XMVector3Equal(oldOrientationVector, orientationVector))
		{
			XMVECTOR rotationAxis = XMVector3Normalize(XMVector3Cross(oldOrientationVector, orientationVector));
			float rotationAmount = XMVectorGetX(XMVector3Dot(oldOrientationVector, orientationVector));
			if (rotationAmount > 1.0) {
				rotationAmount = 1.0;
			}
			else if (rotationAmount < -1.0) {
				rotationAmount = -1.0;
			}
			rotationAmount = acosf(rotationAmount);

			mpTargetTransform->rotate(XMQuaternionRotationAxis(rotationAxis, rotationAmount));

			//std::cout << XMVectorGetX(rotationAxis) << ", " << XMVectorGetY(rotationAxis) << ", " << XMVectorGetZ(rotationAxis) << std::endl;
			//std::cout << rotationAmount << std::endl;
		}
	}
	else if (mpTargetTransform != NULL && hydra->getTrigger(1) > 0.2f)
	{
		XMVECTOR offsetVector =  mHydraPositions[1] - mPreviousHydraPositions[1];

		mpTargetTransform->translate(offsetVector);
	}
	else if (mpTargetTransform != NULL && hydra->getTrigger(0) > 0.2f)
	{
		float oldDistance = XMVectorGetX(XMVector3Length(mPreviousHydraPositions[0] - mPreviousHydraPositions[1]));
		float newDistance = XMVectorGetX(XMVector3Length(mHydraPositions[0] - mHydraPositions[1]));

		float ratio =  newDistance / oldDistance;

		mpTargetTransform->scale(ratio);
	}
}