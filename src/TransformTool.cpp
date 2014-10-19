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

	mHydraPositions[0] = hydra->getPointerPosition(0);
	mHydraPositions[1] = hydra->getPointerPosition(1);

	XMVECTOR hydraAveragePosition = (mHydraPositions[0] + mHydraPositions[1]) * 0.5f;

	static const int bumperBit = 1 << 7;

	if (mpTargetTransform != NULL && (hydra->getButtons(0) & bumperBit) && (hydra->getButtons(1) & bumperBit)) //Rotation
	{
		XMVECTOR oldOrientationVector = XMVector3Normalize(mPreviousHydraPositions[0] - mPreviousHydraPositions[1]);
		XMVECTOR orientationVector = XMVector3Normalize(mHydraPositions[0] - mHydraPositions[1]);

		if (!XMVector3Equal(oldOrientationVector, orientationVector))
		{
			XMVECTOR rotationAxis = XMVector3Normalize(XMVector3Cross(oldOrientationVector, orientationVector));
			float rotationAmount = XMVectorGetX(XMVector3Dot(oldOrientationVector, orientationVector));

			//Avoid undefined values from acosf
			if (rotationAmount > 1.0) 
			{
				rotationAmount = 1.0;
			}
			else if (rotationAmount < -1.0)
			{
				rotationAmount = -1.0;
			}

			rotationAmount = acosf(rotationAmount);
			
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(rotationAxis, rotationAmount);

			//Handle rotations around the grab point
			XMVECTOR newTranslation = mpTargetTransform->getTranslation();
			XMVECTOR translationOffset = newTranslation - hydraAveragePosition;//From point between two controllers to origin of translation
			newTranslation = hydraAveragePosition + XMVector3Rotate(translationOffset, rotationQuaternion); 

			mpTargetTransform->setTranslation(newTranslation);
			mpTargetTransform->rotate(rotationQuaternion);
		}
	}
	else if (mpTargetTransform != NULL && (hydra->getButtons(1) & bumperBit)) //Translation
	{
		XMVECTOR offsetVector =  mHydraPositions[1] - mPreviousHydraPositions[1];

		mpTargetTransform->translate(offsetVector);
	}
	else if (mpTargetTransform != NULL && (hydra->getButtons(0) & bumperBit)) //Scaling
	{
		float oldDistance = XMVectorGetX(XMVector3Length(mPreviousHydraPositions[0] - mPreviousHydraPositions[1]));
		float newDistance = XMVectorGetX(XMVector3Length(mHydraPositions[0] - mHydraPositions[1]));

		float ratio =  newDistance / oldDistance;

		XMVECTOR newTranslation = mpTargetTransform->getTranslation();
		XMVECTOR translationOffset = newTranslation - hydraAveragePosition;//From point between two controllers to origin of translation
		newTranslation = hydraAveragePosition + translationOffset * ratio;

		mpTargetTransform->setTranslation(newTranslation);
		mpTargetTransform->scale(ratio);
	}
}