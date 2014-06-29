#include "Vector3.h"

Vector3::Vector3()
	:mBaseVector(XMVectorZero())
{
}

Vector3::Vector3(const XMVECTOR& vector)
	:mBaseVector(vector)
{
}

Vector3::Vector3(float* pFloatArr)
	:mBaseVector(XMLoadFloat3(&XMFLOAT3(pFloatArr)))
{
}

Vector3::Vector3(const XMFLOAT3& vector)
	:mBaseVector(XMLoadFloat3(&vector))
{
}

Vector3::Vector3(float x, float y, float z)
	:mBaseVector(XMVectorSet(x, y, z, 0.0f))
{
}

float Vector3::length()
{
	return XMVectorGetX(XMVector3Length(mBaseVector));
}

float Vector3::lengthSq()
{
	return XMVectorGetX(XMVector3LengthSq(mBaseVector));
}

void Vector3::normalize()
{
	mBaseVector = XMVector3Normalize(mBaseVector);
}

float Vector3::dot(const Vector3& other)
{
	return XMVectorGetX(XMVector3Dot(mBaseVector, other.mBaseVector));
}

Vector3 Vector3::cross(const Vector3& other)
{
	return XMVector3Cross(mBaseVector, other.mBaseVector);
}

float Vector3::dot(const Vector3& vec1, const Vector3& vec2)
{
	return XMVectorGetX(XMVector3Dot(vec1.mBaseVector, vec2.mBaseVector));
}

Vector3 Vector3::cross(const Vector3& vec1, const Vector3& vec2)
{
	return XMVector3Cross(vec1.mBaseVector, vec2.mBaseVector);
}

Vector3& Vector3::operator=(const XMFLOAT3& other)
{
	mBaseVector = XMLoadFloat3(&other);

	return *this;
}

Vector3& Vector3::operator+=(const Vector3& other)
{
	mBaseVector += other.mBaseVector;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& other)
{
	mBaseVector -= other.mBaseVector;

	return *this;
}

Vector3& Vector3::operator*=(const Vector3& other)
{
	mBaseVector *= other.mBaseVector;

	return *this;
}

Vector3& Vector3::operator/=(const Vector3& other)
{
	mBaseVector /= other.mBaseVector;

	return *this;
}

Vector3& Vector3::operator*=(float scalar)
{
	mBaseVector *= scalar;

	return *this;
}

Vector3& Vector3::operator/=(float scalar)
{
	mBaseVector /= scalar;

	return *this;
}

bool Vector3::operator==(const Vector3& other) const
{
	XMFLOAT3 baseVector1;
	XMFLOAT3 baseVector2;

	XMStoreFloat3(&baseVector1, mBaseVector);
	XMStoreFloat3(&baseVector2, other.mBaseVector);

	return baseVector1.x == baseVector2.x &&
		   baseVector1.y == baseVector2.y &&
		   baseVector1.z == baseVector2.z;
}

bool Vector3::operator!=(const Vector3& other) const
{
	XMFLOAT3 baseVector1;
	XMFLOAT3 baseVector2;

	XMStoreFloat3(&baseVector1, mBaseVector);
	XMStoreFloat3(&baseVector2, other.mBaseVector);

	return baseVector1.x != baseVector2.x ||
		   baseVector1.y != baseVector2.y ||
		   baseVector1.z != baseVector2.z;
}

bool Vector3::operator>(const Vector3& other) const
{
	return XMVectorGetX(XMVector3LengthSq(mBaseVector)) > XMVectorGetX(XMVector3LengthSq(other.mBaseVector));
}

bool Vector3::operator<(const Vector3& other) const
{
	return XMVectorGetX(XMVector3LengthSq(mBaseVector)) < XMVectorGetX(XMVector3LengthSq(other.mBaseVector));
}

bool Vector3::operator>=(const Vector3& other) const
{
	return XMVectorGetX(XMVector3LengthSq(mBaseVector)) >= XMVectorGetX(XMVector3LengthSq(other.mBaseVector));
}

bool Vector3::operator<=(const Vector3& other) const
{
	return XMVectorGetX(XMVector3LengthSq(mBaseVector)) <= XMVectorGetX(XMVector3LengthSq(other.mBaseVector));
}