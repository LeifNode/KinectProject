#pragma once

#include "d3dStd.h"

//Provides an abstraction for the direct3D XMVECTOR
class Vector3
{
public:
	XMVECTOR mBaseVector;

public:
	Vector3();
	Vector3(const XMVECTOR& vector);
	explicit Vector3(float* pFloatArr);
	Vector3(const XMFLOAT3& vector);
	Vector3(float x, float y, float z);

	float getX() const { return XMVectorGetX(mBaseVector); }
	float getY() const { return XMVectorGetY(mBaseVector); }
	float getZ() const { return XMVectorGetZ(mBaseVector); }
	void setX(float x) { mBaseVector = XMVectorSetX(mBaseVector, x); }
	void setY(float y) { mBaseVector = XMVectorSetY(mBaseVector, y); }
	void setZ(float z) { mBaseVector = XMVectorSetZ(mBaseVector, z); }
	_declspec( property( get = getX, put = setX) ) float x;
	_declspec( property( get = getY, put = setY) ) float y;
	_declspec( property( get = getZ, put = setZ) ) float z;

	float length();
	float lengthSq();
	void normalize();

	float dot(const Vector3& other);
	Vector3 cross(const Vector3& other);

	static float dot(const Vector3& vec1, const Vector3& vec2);
	static Vector3 cross(const Vector3& vec1, const Vector3& vec2);

	Vector3 operator+(const Vector3& other) const { return Vector3(mBaseVector + other.mBaseVector); }
	Vector3 operator-(const Vector3& other) const { return Vector3(mBaseVector - other.mBaseVector); }
	Vector3 operator*(const Vector3& other) const { return Vector3(mBaseVector * other.mBaseVector); }
	Vector3 operator/(const Vector3& other) const { return Vector3(mBaseVector / other.mBaseVector); }
	Vector3 operator*(float scalar) const { return Vector3(mBaseVector * scalar); }
	Vector3 operator/(float scalar) const { return Vector3(mBaseVector / scalar); }

	Vector3& operator=(const XMFLOAT3& other);

	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(const Vector3& other);
	Vector3& operator/=(const Vector3& other);

	Vector3& operator*=(float scalar);
	Vector3& operator/=(float scalar);

	bool operator==(const Vector3& other) const;
	bool operator!=(const Vector3& other) const;
	bool operator>(const Vector3& other) const;
	bool operator<(const Vector3& other) const;
	bool operator>=(const Vector3& other) const;
	bool operator<=(const Vector3& other) const;
};