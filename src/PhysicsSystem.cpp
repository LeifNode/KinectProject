#include "PhysicsSystem.h"
#include "InputSystem.h"

PhysicsSystem::PhysicsSystem()
	:mConstraintRenderer(0.01f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f))
{

}

PhysicsSystem::~PhysicsSystem()
{
	for (auto it = mForceGenerators.begin(); it != mForceGenerators.end(); ++it)
	{
		delete (*it);
	}
	
	mForceGenerators.clear();

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		delete (*it);
	}

	mParticles.clear();
}

void PhysicsSystem::Initialize()
{
	mParticleRenderer.Initialize();

	//mAnchor = Vector3(InputSystem::get()->getHydra()->getPointerPosition(1));
	mAnchor = Vector3(0.0f, 2.0f, 0.0f);
	
	mpAnchorParticle = new Particle();
	mpAnchorParticle->setMass(1.0f);
	mpAnchorParticle->setPosition(Vector3(mAnchor));
	mpAnchorParticle->setDamping(0.0f);

	ParticleGravity* gravityForce = new ParticleGravity(Vector3(0.0f, -9.8f, 0.0f));
	ParticleBungee* particleSpring = new ParticleBungee(mpAnchorParticle, 8.1f, 0.1f);

	Particle* newParticle = new Particle();
	newParticle->setMass(0.2f);
	newParticle->setDamping(0.4f);
	newParticle->setPosition(mAnchor + Vector3(0.0f, 2.0f, 0.0f));

	//Particle* newParticleChain1 = new Particle();
	//newParticleChain1->setMass(0.1f);
	//newParticleChain1->setDamping(0.9f);
	//newParticleChain1->setPosition(Vector3(1.0f, 3.5f, 0.0f));

	//ParticleSpring* bungee1 = new ParticleSpring(newParticle, 3.1f, 1.0f);
	//ParticleSpring* bungee2 = new ParticleSpring(newParticleChain1, 3.1f, 1.0f);

	mForceRegistry.add(newParticle, gravityForce);
	mForceRegistry.add(newParticle, particleSpring);
	//mForceRegistry.add(newParticleChain1, gravityForce);
	//mForceRegistry.add(newParticleChain1, bungee1);
	//mForceRegistry.add(newParticle, bungee2);

	mConstraintRenderer.addConstraint(mpAnchorParticle, newParticle);
	//mConstraintRenderer.addConstraint(newParticle, newParticleChain1);
	mParticleRenderer.addParticle(newParticle);
	//mParticleRenderer.addParticle(newParticleChain1);

	mParticles.push_back(newParticle);
	mParticles.push_back(mpAnchorParticle);
	//mParticles.push_back(newParticleChain1);

	mForceGenerators.push_back(particleSpring);
	mForceGenerators.push_back(gravityForce);
	//mForceGenerators.push_back(bungee1);
	//mForceGenerators.push_back(bungee2);
}

void PhysicsSystem::Update(float dt)
{
	mAnchor = Vector3(InputSystem::get()->getHydra()->getPointerPosition(1));
	mpAnchorParticle->setPosition(mAnchor);

	mForceRegistry.update(dt);

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->integrate(dt);
	}

	mConstraintRenderer.Update(dt);
}

void PhysicsSystem::Render(D3DRenderer* renderer)
{
	mParticleRenderer.Render(renderer);
	mConstraintRenderer.Render(renderer);
}