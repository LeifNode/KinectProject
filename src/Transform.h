#pragma once

#include "d3dStd.h"

class Transform
{
public:
	Transform();
	//Transform(const XMMATRIX& transform, const XMVECTOR& rotationQuaternion);
	~Transform();

	void rotate(const XMVECTOR& rotationQuaternion);
	
	void scale(float scale);
	void scale(float x, float y, float z);
	void scale(const XMVECTOR& scalingAxis);

	void translate(const XMVECTOR& offset);

	XMMATRIX getTransform();
private:
	XMMATRIX mTransformation;
	XMVECTOR mRotationQuat;
	XMVECTOR mRotationOrigin;
	XMVECTOR mScaling;
	XMVECTOR mTranslation;
};