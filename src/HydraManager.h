#pragma once

#include "d3dStd.h"
#include <sixense.h>
#include <sixense_math.hpp>
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

class D3DRenderer;

class HydraManager
{
public:
	HydraManager();
	~HydraManager();

	void Initialize();

	void Update(float dt);

	float getPointerDistance() const { return mPointerDistance; }
	void setPointerDistance(float distance) { mPointerDistance = distance; }

	XMVECTOR getPosition(int controllerIndex) const;
	XMVECTOR getRotation(int controllerIndex) const;

	XMVECTOR getPointerPosition(int controllerIndex) const;

	unsigned int getButtons(int controllerIndex) const;

	float getTrigger(int controllerIndex) const;

	XMFLOAT2 getJoystick(int controllerIndex) const;

	bool isDocked(int controllerIndex) const;

private:
	sixenseAllControllerData mAcd;


	float mPointerDistance;
};