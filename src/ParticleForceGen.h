#pragma once

#include "Particle.h"

class ParticleForceGenerator
{
public:
	virtual void updateForce(Particle* particle, float dt) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
public:
	ParticleGravity(const Vector3& gravity);

	virtual void updateForce(Particle* particle, float dt);

private:
	Vector3 mGravity;
};

class ParticleDrag : public ParticleForceGenerator
{
public:
	ParticleDrag(float k1, float k2);

	virtual void updateForce(Particle* particle, float dt);

private:
	float mK1, mK2;
};

class ParticleSpring : public ParticleForceGenerator
{
public:
	ParticleSpring(Particle* other, float springConstant, float restLength);

	virtual void updateForce(Particle* particle, float dt);

private:
	Particle* other;

	float springConstant;

	float restLength;
};

class ParticleAnchoredSpring : public ParticleForceGenerator
{
public:
	ParticleAnchoredSpring(Vector3* anchor, float springConstant, float restLength);

	virtual void updateForce(Particle* particle, float dt);

protected:
	Vector3* anchor;

	float springConstant;
	float restLength;
};

class ParticleBungee : public ParticleForceGenerator
{
public:
	ParticleBungee(Particle* other, float springConstant, float restLength);

	virtual void updateForce(Particle* particle, float dt);

private:
	Particle* other;

	float springConstant;
	float restLength;
};

class ParticleForceRegistry
{
protected:
	struct ParticleForceRegistration
	{
		Particle* particle;
		ParticleForceGenerator* fg;
	};

	std::vector<ParticleForceRegistration> registrations;

public:
	void add(Particle* particle, ParticleForceGenerator* fg);

	void clear();

	void update(float dt);
};