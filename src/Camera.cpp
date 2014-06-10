#include "Camera.h"
#include "MathHelper.h"
#include "InputSystem.h"

Camera::Camera(const XMFLOAT3& position)
	:mPosition(position),
	mReadingMouse(false),
	mVelocity(5.0f),
	mFreeRoam(true)
{
	mDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mUp = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	mRotation = XMQuaternionIdentity();
	mNear = 0.1f;
	mFar = 10000.0f;
	OnResize(1280, 720);
}

Camera::~Camera()
{

}

XMMATRIX Camera::getView(const XMVECTOR& offset, const XMVECTOR& rotationQuat)
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMVECTOR up = XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationQuat);
	XMVECTOR forward = XMVector3Rotate(XMLoadFloat3(&mDirection), rotationQuat);

	return XMMatrixLookAtLH(pos + offset, pos + offset + forward, up);
}

void Camera::OnMouseMove(int x, int y)
{
	if (mReadingMouse)
	{
		int mouseDx = mLastMousePosition.x - x;
		int mouseDy = mLastMousePosition.y - y;

		float dx = mouseDx * (MathHelper::Pi / 180.0f) * 0.2f;
		float dy = mouseDy * (MathHelper::Pi / 180.0f) * 0.2f;

		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(direction, mUp));

#if !USE_RIFT
		XMVECTOR rotationQuat =  XMQuaternionMultiply(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), -dx), XMQuaternionRotationAxis(right, dy));
#else
		XMVECTOR rotationQuat =  XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), -dx);
#endif

		mRotation = XMQuaternionNormalize(XMQuaternionMultiply(mRotation, rotationQuat));

		XMStoreFloat3(&mDirection, XMVector3Normalize(XMVector3Rotate(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f)), mRotation)));
	}

	mLastMousePosition.x = x;
	mLastMousePosition.y = y;
}

void Camera::OnMouseDown(MouseButton button)
{
	if (button == MOUSE_LEFT)
		mReadingMouse = true;
	if (button == MOUSE_RIGHT)
		mFreeRoam = !mFreeRoam;
}

void Camera::OnMouseUp(MouseButton button)
{
	if (button == MOUSE_LEFT)
		mReadingMouse = false;
}

void Camera::OnResize(int width, int height)
{
	setProjection(width, height, 45.0f);
}

void Camera::Update(float dt)
{
	updateOrientation(dt);
	updatePosition(dt);
}

void Camera::updatePosition(float dt)
{
	InputSystem* inputSystem = InputSystem::get();
	HydraManager* hydra = inputSystem->getHydra();

	XMVECTOR forwardOffset = XMLoadFloat3(&getDirection()) * mVelocity * dt;
	XMVECTOR sidewaysOffset = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&getDirection()), mUp)) * mVelocity * dt;

	if (!mFreeRoam)
	{
		//Add to position based on direction
		//XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + forwardOffset);

		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_W))
		{
			mVelocity *= powf(2.0f, dt * 2.0f);
		}
		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_S))
		{
			mVelocity *= powf(0.5f, dt * 2.0f);
		}
	}
	else
	{
		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_W))
		{
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + forwardOffset);
		}
		else
		{
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + forwardOffset * hydra->getJoystick(0).y);
		}

		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_S))
		{
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) - forwardOffset);
		}

		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_A))
		{
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + sidewaysOffset);
		}
		else
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) - sidewaysOffset * hydra->getJoystick(0).x);

		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_D))
		{
			XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) - sidewaysOffset);
		}
	}
}

void Camera::updateOrientation(float dt)
{
	InputSystem* inputSystem = InputSystem::get();
	HydraManager* hydra = inputSystem->getHydra();


	float dx = hydra->getJoystick(1).x * (MathHelper::Pi / 180.0f) * 0.2f;
	float dy = hydra->getJoystick(1).y * (MathHelper::Pi / 180.0f) * 0.2f;

	XMVECTOR direction = XMLoadFloat3(&mDirection);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(direction, mUp));

#if !USE_RIFT
	XMVECTOR rotationQuat =  XMQuaternionMultiply(XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), dx), XMQuaternionRotationAxis(right, dy));
#else
	XMVECTOR rotationQuat =  XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), dx);
#endif

	mRotation = XMQuaternionNormalize(XMQuaternionMultiply(mRotation, rotationQuat));

	XMStoreFloat3(&mDirection, XMVector3Normalize(XMVector3Rotate(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f)), mRotation)));

	if (!mFreeRoam)
	{
		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_A))
		{
			mUp = XMVector3Normalize(XMVector3Rotate(mUp, XMQuaternionRotationAxis(XMLoadFloat3(&mDirection), 1.0f * dt)));
		}
		if (inputSystem->getKeyboardState()->isKeyPressed(KEY_D))
		{
			mUp = XMVector3Normalize(XMVector3Rotate(mUp, XMQuaternionRotationAxis(XMLoadFloat3(&mDirection), -1.0f * dt)));
		}
	}
}

void Camera::setProjection(int width, int height, float fovVertical)
{
	mProjection = XMMatrixPerspectiveFovLH(45.0f, (float)width / (float)height, mNear, mFar);
}