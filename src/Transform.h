#pragma once

#include "d3dStd.h"

class Transform
{
public:
	Transform();
	~Transform();

	void rotate(const XMVECTOR& rotationQuaternion);

	XMVECTOR getRotation() const { return mRotationQuat; }
	void setRotation(const XMVECTOR& rotation) { mRotationQuat = rotation; }
	
	void scale(float scale);
	void scale(float x, float y, float z);
	void scale(const XMVECTOR& scalingAxis);

	void translate(const XMVECTOR& offset);

	void setTranslation(const XMVECTOR& translation) { mTranslation = translation; }
	XMVECTOR getTranslation() const { return mTranslation; }

	XMMATRIX getTransform();

private:
	XMMATRIX mTransformation;
	XMVECTOR mRotationQuat;
	XMVECTOR mRotationOrigin;
	XMVECTOR mScaling;
	XMVECTOR mTranslation;
};