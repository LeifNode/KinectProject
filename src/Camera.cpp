//#include "Camera.h"
//#include "MathHelper.h"
//#include "InputSystem.h"
//
////Camera::Camera()
////	:mPosition(0.0f),
////	mUp(0.0f, 1.0f, 0.0f),
////	mReadingMouse(false),
////	mDirectionChanged(true),
////	mYaw(0.0f),
////	mPitch(0.0f)
////{
////	mNear = 0.1f;
////	mFar = 300000000.0f;
////	setProjection(1280, 720, 45.0f);
////	setDirection(vec3(1.0f, 0.0f, 0.0f));
////}
//
//Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up)
//	:mPosition(position),
//	mUp(up),
//	mReadingMouse(false),
//	mVelocity(100.0f),
//	mRotation(),
//	mFreeRoam(true)
//{
//	mNear = 0.1f;
//	mFar = 300000000.0f;
//	setProjection(1280, 720, 45.0f);
//}
//
//Camera::~Camera()
//{
//
//}
//
//XMMATRIX Camera::getView()
//{
//	vec3 currentDirection = getDirection();
//
//	return getLookAt(vec3(mPosition), getDirection(), mUp);
//}
//
////When very large values were used as the camera position the looking direction controls became 
////  fairly broken because of the vector subtraction that was in the glm::lookat function
//XMMATRIX Camera::getLookAt(const XMFLOAT3& eye, const XMFLOAT3& direction, const XMFLOAT3& up)
//{
//	/*XMVECTOR f(glm::normalize(direction));
//	XMVECTOR s(glm::normalize(glm::cross(f, up)));
//	XMVECTOR u(glm::cross(s, f));*/
//
//	XMFLOAT4X4 Result;
//	Result[0][0] = s.x;
//	Result[1][0] = s.y;
//	Result[2][0] = s.z;
//	Result[0][1] = u.x;
//	Result[1][1] = u.y;
//	Result[2][1] = u.z;
//	Result[0][2] =-f.x;
//	Result[1][2] =-f.y;
//	Result[2][2] =-f.z;
//	Result[3][0] =-glm::dot(s, eye);
//	Result[3][1] =-glm::dot(u, eye);
//	Result[3][2] = glm::dot(f, eye);
//	return Result;
//}
//
//void Camera::setDirection(const vec3& newDir)
//{
//	mDirection = glm::normalize(newDir);
//
//	mPitch = acosf(newDir.z);
//	mYaw = atanf(newDir.y / newDir.x);
//
//	mAdjustedDirection = mRotation * mDirection;
//}
//
//void Camera::setPitch(float pitch)
//{
//	float angleLimit = 80.0f * MathUtils::Pi / 180.0f;
//	mPitch = MathUtils::Clamp(pitch, -angleLimit, angleLimit);
//	mPitch = pitch;
//
//	mDirectionChanged = true;
//}
//
//vec3 Camera::getDirection()
//{
//	if (mDirectionChanged)
//	{
//		/*vec3 forward = vec3(0.0f, 0.0f, 1.0f);
//		glm::quat rotation(vec3(mPitch, mYaw, 0.0f));
//
//		forward = rotation * forward;*/
//
//		mDirection.x = cosf(mYaw) * sinf(mPitch);
//		mDirection.y = cosf(mPitch);
//		mDirection.z = sinf(mYaw) * sinf(mPitch);
//
//		mAdjustedDirection = mRotation * mDirection;
//
//		mDirectionChanged = false;
//	}
//
//	return mAdjustedDirection;
//}
//
//void Camera::OnMouseMove(float x, float y)
//{
//	if (mReadingMouse)
//	{
//		float dx = mLastMousePosition.x - x;
//		float dy = mLastMousePosition.y - y;
//
//		dx = dx * (MathHelper::Pi / 180.0f) * 0.2f;
//		dy = dy * (MathHelper::Pi / 180.0f) * 0.2f;
//
//		//mRotation = glm::rotate(mRotation, dx * 100.0f, mRotation * vec3(0.0f, 1.0f, 0.0f));
//		//mRotation = glm::rotate(mRotation, dy * 100.0f, mRotation * vec3(0.0f, 0.0f, 1.0f));
//
//		//mUp = mRotation * vec3(0.0f, 1.0f, 0.0f);
//
//		pitch -= dy;
//		yaw -= dx;
//	}
//
//	mLastMousePosition.x = x;
//	mLastMousePosition.y = y;
//}
//
//void Camera::OnMouseDown(int button)
//{
//	if (button == 0)
//		mReadingMouse = true;
//	if (button == 1)
//		mFreeRoam = !mFreeRoam;
//}
//
//void Camera::OnMouseUp(int button)
//{
//	if (button == 0)
//		mReadingMouse = false;
//}
//
//void Camera::OnScreenResize(int width, int height)
//{
//	setProjection(width, height, 45.0f);
//}
//
//void Camera::Update(float dt)
//{
//	updateOrientation(dt);
//	updatePosition(dt);
//}
//
//void Camera::updatePosition(float dt)
//{
//	if (!mFreeRoam)
//	{
//		mPosition += dvec3(mAdjustedDirection * mVelocity * dt);
//
//		if ()
//		{
//			mVelocity *= powf(2.0, dt * 2.0);
//		}
//		if (glfwGetKey(mpWindow, GLFW_KEY_S) == GLFW_PRESS)
//		{
//			mVelocity *= powf(0.5, dt * 2.0);
//		}
//	}
//	else
//	{
//		if (glfwGetKey(mpWindow, GLFW_KEY_W) == GLFW_PRESS)
//		{
//			mPosition += dvec3(mAdjustedDirection * mVelocity * dt);
//		}
//		if (glfwGetKey(mpWindow, GLFW_KEY_S) == GLFW_PRESS)
//		{
//			mPosition -= dvec3(mAdjustedDirection * mVelocity * dt);
//		}
//		if (glfwGetKey(mpWindow, GLFW_KEY_A) == GLFW_PRESS)
//		{
//			mPosition -= dvec3(glm::cross(mAdjustedDirection, mUp) * mVelocity * dt);
//		}
//		if (glfwGetKey(mpWindow, GLFW_KEY_D) == GLFW_PRESS)
//		{
//			mPosition += dvec3(glm::cross(mAdjustedDirection, mUp) * mVelocity * dt);
//		}
//	}
//}
//
//void Camera::updateOrientation(float dt)
//{
//	if (!mFreeRoam)
//	{
//		if (glfwGetKey(mpWindow, GLFW_KEY_A) == GLFW_PRESS)
//		{
//			mRotation = glm::rotate(mRotation, -40.0f * dt, mDirection);
//		}
//		if (glfwGetKey(mpWindow, GLFW_KEY_D) == GLFW_PRESS)
//		{
//			mRotation = glm::rotate(mRotation, 40.0f * dt, mDirection);
//		}
//	}
//
//	mUp = mRotation * vec3(0.0f, 1.0f, 0.0f); 
//}
//
//void Camera::setProjection(int width, int height, float fovVertical)
//{
//	mProjection = XMMatrixPerspectiveFovLH(45.0f, (float)width / (float)height, mNear, mFar);
//}