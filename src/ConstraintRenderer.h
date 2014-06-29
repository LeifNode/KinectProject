#pragma once

#include "d3dStd.h"
#include "Renderable.h"
#include "LineRenderer.h"

class Particle;
class D3DRenderer;

class ConstraintRenderer : public Renderable
{
public:
	ConstraintRenderer(float lineThickness, const XMFLOAT4& color);
	~ConstraintRenderer();

	void addConstraint(Particle* p1, Particle* p2);
	void removeConstraint(Particle* p1, Particle* p2);
	void removeConnectedConstraints(Particle* particle);

	void Update(float dt);
	void Render(D3DRenderer* renderer);

private:
	LineRenderer mLineRenderer;

	std::list<std::pair<Particle*, Particle*>> mParticleReferences; 
};