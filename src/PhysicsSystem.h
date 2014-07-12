#pragma once

#include "d3dStd.h"
#include "Particle.h"
#include "ParticleForceGen.h"
#include "ParticleRenderer.h"
#include "ConstraintRenderer.h"
#include "ParticleContactGenerators.h"

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
	unsigned generateContacts();

	void createBox();
	void addRod(Particle* p1, Particle* p2, float length);

private:
	Vector3 mAnchor;
	Particle* mpAnchorParticle;

	std::vector<Particle*> mParticles;

	std::list<ParticleForceGenerator*> mForceGenerators;
	ParticleForceRegistry mForceRegistry;

	std::vector<ParticleContactGenerator*> mContactGenerators;

	ParticleContact* mpContactArr;
	unsigned mMaxContacts;

	ParticlePlane mPlaneCollider;

	ParticleContactResolver mResolver;

	ParticleRenderer mParticleRenderer;
	ConstraintRenderer mConstraintRenderer;
};