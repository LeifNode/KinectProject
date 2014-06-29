#pragma once

#include "d3dStd.h"
#include "Particle.h"
#include "ParticleForceGen.h"
#include "ParticleRenderer.h"
#include "ConstraintRenderer.h"

class D3DRenderer;

class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Initialize();

	void Update(float dt);

	void Render(D3DRenderer* renderer);

private:
	Vector3 mAnchor;
	Particle* mpAnchorParticle;

	std::vector<Particle*> mParticles;

	std::list<ParticleForceGenerator*> mForceGenerators;
	ParticleForceRegistry mForceRegistry;

	ParticleRenderer mParticleRenderer;
	ConstraintRenderer mConstraintRenderer;
};