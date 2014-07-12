#pragma once

#include "Particle.h"

class ParticleContact
{
	friend class ParticleContactResolver;
public:
	ParticleContact();

	Particle* particle[2];

	float restitution;

	Vector3 contactNormal;

	float penetration;

	Vector3 particleMovement[2];

protected:
	void resolve(float dt);

	float calculateSeparatingVelocity() const;

private:
	void resolveVelocity(float dt);
	void resolveInterpenetration(float dt);
};

class ParticleContactResolver
{
public:
	ParticleContactResolver(unsigned iterations);

	void setIterations(unsigned iterations);

	void resolveContacts(ParticleContact* contactArr, unsigned numContacts, float dt);

protected:
	unsigned iterations;

	//For perfromance tracking
	unsigned iterationsUsed;
};