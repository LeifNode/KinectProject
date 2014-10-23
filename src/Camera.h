#pragma once

#include "d3dStd.h"
#include "InputSystem.h"

class Camera
{
public:
	//Camera();
	Camera(const XMFLOAT3& position);
	~Camera();

	XMMATRIX getView(const XMVECTOR& offset, const XMVECTOR& rotationQuat);
	virtual XMMATRIX getProj() const { return mProjection; }

	//__declspec(property(get = getPosition, put = setPosition)) XMFLOAT3 position; //Causes runtime errors when this class has virtual functions
	XMFLOAT3 getPosition() const { return mPosition; }
	void setPosition(const XMFLOAT3& newPosition) { mPosition = newPosition; }

	float getNear() const { return mNear; }
	float getFar() const { return mFar; }

	float getVelocity() const { return mVelocity; }
	XMFLOAT3 getDirection() const { return mDirection; }

	void OnMouseMove(int x, int y);
	void OnMouseDown(MouseButton button);
	void OnMouseUp(MouseButton button);
	void OnResize(int width, int height);

	void Update(float dt);

private:
	void updatePosition(float dt);
	void updateOrientation(float dt);

	void setProjection(int width, int height, float fovVertical);

private:
	XMMATRIX mProjection;

	XMFLOAT3 mPosition;
	XMFLOAT3 mDirection;
	XMVECTOR mRotation;
	XMVECTOR mUp;

	float mNear;
	float mFar;
	float mVelocity;

	XMINT2 mLastMousePosition;

	bool mReadingMouse;
	bool mFreeRoam;
};