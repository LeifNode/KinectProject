#pragma once

#include "d3dStd.h"
#include "Renderable.h"
#include "MeshRenderer.h"
#include "Geometry.h"
#include "Particle.h"

class ParticleRenderer : public Renderable
{
public:
	ParticleRenderer();
	~ParticleRenderer();

	void Initialize();

	void addParticle(Particle* particle);
	void removeParticle(Particle* particle);

	virtual void Render(D3DRenderer* renderer);

private:
	MeshRenderer<Vertex> mMeshRenderer;

	std::list<Particle*> mTargetParticles;
};