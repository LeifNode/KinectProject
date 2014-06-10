#pragma once

#include "d3dStd.h"
#include "Transform.h"
#include "InputSystem.h"

class TransformTool
{
public:
	TransformTool();
	~TransformTool();

	void setTargetTransform(Transform* transform) { mpTargetTransform = transform; };

	void Update(float dt);
private:
	Transform* mpTargetTransform;

	XMVECTOR mPreviousHydraPositions[2];
	XMVECTOR mHydraPositions[2];
};