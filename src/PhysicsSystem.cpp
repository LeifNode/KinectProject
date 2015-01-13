#include "PhysicsSystem.h"
#include "InputSystem.h"

PhysicsSystem::PhysicsSystem()
	:mConstraintRenderer(0.01f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
	mResolver(100)
{
	mMaxContacts = 1000;
	mpContactArr = new ParticleContact[mMaxContacts]();
}

PhysicsSystem::~PhysicsSystem()
{
	for (auto it = mForceGenerators.begin(); it != mForceGenerators.end(); ++it)
	{
		SAFE_DELETE((*it));
	}
	
	mForceGenerators.clear();

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		SAFE_DELETE((*it));
	}

	mParticles.clear();

	for (auto it = mContactGenerators.begin(); it != mContactGenerators.end(); ++it)
	{
		SAFE_DELETE((*it));
	}

	mContactGenerators.clear();

	SAFE_DELETEARR(mpContactArr);
}

void PhysicsSystem::Initialize()
{
	mParticleRenderer.Initialize();

	//mPlaneCollider.restitution = 0.5f;
	//mPlaneCollider.pParticles = &mParticles;

	ParticlePlane* particlePlane = new ParticlePlane();
	particlePlane->pParticles = &mParticles;
	particlePlane->restitution = 0.9f;

	mContactGenerators.push_back(particlePlane);

	//mAnchor = Vector3(InputSystem::get()->getHydra()->getPointerPosition(1));
	mAnchor = Vector3(0.0f, 2.0f, 0.0f);
	
	mpAnchorParticle = new Particle();
	mpAnchorParticle->setMass(1.0f);
	mpAnchorParticle->setPosition(Vector3(mAnchor));
	mpAnchorParticle->setDamping(0.0f);

	ParticleGravity* gravityForce = new ParticleGravity(Vector3(0.0f, -9.8f, 0.0f));
	ParticleBungee* particleSpring = new ParticleBungee(mpAnchorParticle, 2.1f, 0.1f);

	Particle* newParticle = new Particle();
	newParticle->setMass(0.2f);
	newParticle->setDamping(0.4f);
	newParticle->setPosition(mAnchor - Vector3(0.0f, 0.5f, 0.0f));

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

	for (int i = 0; i < 10; i++)
	{
		Particle* particle = new Particle();
		particle->setMass(0.2f);
		particle->setDamping(0.4f);
		particle->setPosition(mAnchor - Vector3(MathHelper::RandF(-2.0f, 2.0f), 0.5f + MathHelper::RandF(-0.1f, 1.0f), MathHelper::RandF(-2.0f, 2.0f)));

		mParticles.push_back(particle);
		mParticleRenderer.addParticle(particle);
		mForceRegistry.add(particle, gravityForce);
	}

	createBox();
}

void PhysicsSystem::Update(float dt)
{
	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->clearAccmulator();
	}

	mAnchor = Vector3(InputSystem::get()->getHydra()->getPointerPosition(1));
	mpAnchorParticle->setPosition(mAnchor);

	mForceRegistry.update(dt);

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->integrate(dt);
	}

	unsigned usedContacts = generateContacts();

	if (usedContacts)
	{
		mResolver.resolveContacts(mpContactArr, usedContacts, dt);
	}

	mConstraintRenderer.Update(dt);
}

void PhysicsSystem::Render(D3DRenderer* renderer)
{
	mParticleRenderer.Render(renderer);
	mConstraintRenderer.Render(renderer);
}

unsigned PhysicsSystem::generateContacts()
{
	unsigned limit = mMaxContacts;
	ParticleContact *nextContact = mpContactArr;

	for (auto g = mContactGenerators.begin();
        g != mContactGenerators.end();
        g++)
    {
        unsigned used =(*g)->addContact(nextContact, limit);
        limit -= used;
        nextContact += used;

        // We've run out of contacts to fill. This means we're missing
        // contacts.
        if (limit <= 0) break;
    }

    // Return the number of contacts used.
	return mMaxContacts - limit;
}


void PhysicsSystem::createBox()
{
	ParticleGravity* gravityForce = new ParticleGravity(Vector3(0.0f, -9.8f, 0.0f));

	Particle* particleArray[8];

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int z = 0; z < 2; z++)
			{
				Particle* boxParticle = new Particle();
				boxParticle->setMass(0.01f);
				boxParticle->setDamping(0.9f);
				boxParticle->setPosition(Vector3((float)x + 0.1f * (float)y, (float)y + 1, (float)z));

				mParticleRenderer.addParticle(boxParticle);
				mParticles.push_back(boxParticle);
				mForceRegistry.add(boxParticle, gravityForce);

				particleArray[x * 4 + y * 2 + z] = boxParticle;
			}
		}
	}

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			addRod(particleArray[x * 4 + y * 2], particleArray[x * 4 + y * 2 + 1], 1.0f);
		}
	}

	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			addRod(particleArray[x * 4 + z], particleArray[x * 4 + z + 2], 1.0f);
		}
	}

	for (int y = 0; y < 2; y++)
	{
		for (int z = 0; z < 2; z++)
		{
			addRod(particleArray[y * 2 + z], particleArray[y * 2 + z + 4], 1.0f);
		}
	}

	addRod(particleArray[0], particleArray[7], 1.73205f);
	addRod(particleArray[1], particleArray[6], 1.73205f);
	addRod(particleArray[2], particleArray[5], 1.73205f);
	addRod(particleArray[3], particleArray[4], 1.73205f);

	addRod(particleArray[0], particleArray[3], 1.41421f);
	addRod(particleArray[4], particleArray[7], 1.41421f);
	addRod(particleArray[2], particleArray[7], 1.41421f);
	addRod(particleArray[1], particleArray[4], 1.41421f);
}

void PhysicsSystem::addRod(Particle* p1, Particle* p2, float length)
{
	ParticleRod* newRod = new ParticleRod();
	newRod->length = length;
	newRod->particle[0] = p1;
	newRod->particle[1] = p2;

	mContactGenerators.push_back(newRod);

	mConstraintRenderer.addConstraint(newRod->particle[0], newRod->particle[1]);
}