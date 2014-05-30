//#pragma once
//
//#include "d3dStd.h"
//
//class Camera
//{
//public:
//	//Camera();
//	Camera(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up);
//	~Camera();
//
//	XMMATRIX getView();
//	XMMATRIX getProj() const { return mProjection; }
//
//	__declspec(property(get = getPosition, put = setPosition)) XMFLOAT3 position;
//	XMFLOAT3 getPosition() const { return mPosition; }
//	void setPosition(const XMFLOAT3& newPosition) { mPosition = newPosition; }
//
//	float getNear() const { return mNear; }
//	float getFar() const { return mFar; }
//
//	float getVelocity() const { return mVelocity; }
//
//	void OnMouseMove(float x, float y);
//	void OnMouseDown(int button);
//	void OnMouseUp(int button);
//	void OnScreenResize(int width, int height);
//
//	void Update(float dt);
//
//private:
//	void updatePosition(float dt);
//	void updateOrientation(float dt);
//
//	void setProjection(int width, int height, float fovVertical);
//	XMMATRIX getLookAt(const XMFLOAT3& eye, const XMFLOAT3& direction, const XMFLOAT3& up);
//
//private:
//	XMMATRIX mProjection;
//
//	XMFLOAT3 mPosition;
//	XMFLOAT3 mDirection;
//	XMFLOAT3 mAdjustedDirection;
//	XMFLOAT3 mUp, mRight;
//	XMFLOAT4 mRotation;
//
//	float mNear;
//	float mFar;
//	float mVelocity;
//
//	XMFLOAT2 mLastMousePosition;
//
//	bool mReadingMouse;
//	bool mFreeRoam;
//};