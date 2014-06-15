#include "Transform.h"

Transform::Transform()
	:mTransformation(XMMatrixIdentity()),
	mRotationQuat(XMQuaternionIdentity()),
	mRotationOrigin(XMVectorZero()),
	mScaling(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
	mTranslation(XMVectorZero())
{

}

Transform::~Transform()
{
}

void Transform::rotate(const XMVECTOR& rotationQuaternion)
{
	mRotationQuat = XMQuaternionNormalize(XMQuaternionMultiply(mRotationQuat, rotationQuaternion));
}
	
void Transform::scale(float scale)
{
	mScaling *= scale;
}

void Transform::scale(float x, float y, float z)
{
	XMVECTOR scaleVec = XMVectorSet(x, y, z, 0.0f);
	mScaling *= scaleVec;
}

void Transform::scale(const XMVECTOR& scalingAxis)
{
	mScaling *= XMVector3Rotate(scalingAxis, mRotationQuat);
}

void Transform::setScale(float scale)
{
	mScaling = XMVectorSet(scale, scale, scale, 0.0f);
}

void Transform::setScale(float scaleX, float scaleY, float scaleZ)
{
	mScaling = XMVectorSet(scaleX, scaleY, scaleZ, 0.0f);
}

void Transform::setScale(const XMVECTOR& scaleVec)
{
	mScaling = scaleVec;
}

void Transform::translate(const XMVECTOR& offset)
{
	mTranslation += offset;
}

XMMATRIX Transform::getTransform()
{
	return XMMatrixAffineTransformation(mScaling, mRotationOrigin, mRotationQuat, mTranslation);
}

void Transform::reset()
{
	mScaling = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	mRotationQuat = XMQuaternionIdentity();
	mTranslation = XMVectorZero();
	mRotationOrigin = XMVectorZero();
}