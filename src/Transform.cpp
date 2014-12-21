#include "Transform.h"

Transform::Transform()
	:mTransformation(XMMatrixIdentity()),
	mRotationQuat(XMQuaternionIdentity()),
	mRotationOrigin(XMVectorZero()),
	mScaling(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
	mTranslation(XMVectorZero()),
	mTransformDirty(true)
{

}

Transform::~Transform()
{
}

void Transform::rotate(const XMVECTOR& rotationQuaternion)
{
	mTransformDirty = true;
	mRotationQuat = XMQuaternionNormalize(XMQuaternionMultiply(mRotationQuat, rotationQuaternion));
}
	
void Transform::scale(float scale)
{
	mTransformDirty = true;
	mScaling *= scale;
}

void Transform::scale(float x, float y, float z)
{
	mTransformDirty = true;
	XMVECTOR scaleVec = XMVectorSet(x, y, z, 0.0f);
	mScaling *= scaleVec;
}

void Transform::scale(const XMVECTOR& scalingAxis)
{
	mTransformDirty = true;
	mScaling *= XMVector3Rotate(scalingAxis, mRotationQuat);
}

void Transform::setScale(float scale)
{
	mTransformDirty = true;
	mScaling = XMVectorSet(scale, scale, scale, 1.0f);
}

void Transform::setScale(float scaleX, float scaleY, float scaleZ)
{
	mTransformDirty = true;
	mScaling = XMVectorSet(scaleX, scaleY, scaleZ, 1.0f);
}

void Transform::setScale(const XMVECTOR& scaleVec)
{
	mTransformDirty = true;
	mScaling = scaleVec;
}

void Transform::translate(const XMVECTOR& offset)
{
	mTransformDirty = true;
	mTranslation += offset;
}

XMMATRIX Transform::getTransform()
{
	if (mTransformDirty)
	{
		mTransformation = XMMatrixAffineTransformation(mScaling, mRotationOrigin, mRotationQuat, mTranslation);
		mTransformDirty = false;
	}

	return mTransformation;
}

void Transform::reset()
{
	mScaling = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	mRotationQuat = XMQuaternionIdentity();
	mTranslation = XMVectorZero();
	mRotationOrigin = XMVectorZero();
	mTransformDirty = true;
}