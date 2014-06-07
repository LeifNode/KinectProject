#pragma once

#include "d3dStd.h"
#include <sixense.h>
#include <sixense_math.hpp>
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

class D3DRenderer;
template <class T>
class MeshRenderer;
struct Vertex;

class HydraManager
{
public:
	HydraManager();
	~HydraManager();

	void Initialize();

	void Update(float dt);
	void Render(D3DRenderer* renderer);

private:
	MeshRenderer<Vertex>* mpPointerRenderer;
	MeshRenderer<Vertex>* mpRootRenderer;
};