#include "Transform.h"

Transform::Transform()
	:mTransformation(XMMatrixIdentity()),
	mRotationQuat(XMQuaternionIdentity()),
	mRotationOrigin(XMVectorZero()),
	mScaling(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
	mTranslation(XMVectorZero())
{

}

//Transform::Transform(const XMMATRIX& transform, const XMVECTOR& rotationQuaternion)
//	:mTransformation(transform),
//	mRotationQuat(rotationQuaternion)
//{
//
//}

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

void Transform::translate(const XMVECTOR& offset)
{
	//mTransformation = mTransformation * XMMatrixTranslationFromVector(offset);
	mTranslation += offset;
}

XMMATRIX Transform::getTransform()
{
	return XMMatrixAffineTransformation(mScaling, mRotationOrigin, mRotationQuat, mTranslation);
}