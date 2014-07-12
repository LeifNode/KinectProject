#include "ParticleForceGen.h"
#include "MathHelper.h"

ParticleGravity::ParticleGravity(const Vector3& gravity)
	:mGravity(gravity)
{
}

void ParticleGravity::updateForce(Particle* particle, float dt)
{
	if (!particle->hasFiniteMass())
		return;

	particle->addForce(mGravity * particle->getMass());
}

ParticleDrag::ParticleDrag(float k1, float k2)
	:mK1(k1),
	mK2(k2)
{}

void ParticleDrag::updateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getVelocity();

	float dragCoeff = force.length();
	dragCoeff = mK1 * dragCoeff + mK2 * dragCoeff * dragCoeff;

	force.normalize();
	force *= -dragCoeff;

	particle->addForce(force);
}

ParticleSpring::ParticleSpring(Particle* other, float springConstant, float restLength)
	:other(other),
	springConstant(springConstant),
	restLength(restLength)
{

}

void ParticleSpring::updateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition();

	force -= other->getPosition();

	float magnitude = force.length();
	magnitude = fabsf(magnitude - restLength);
	magnitude *= springConstant;

	force.normalize();
	force *= -magnitude;

	particle->addForce(force);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vector3* anchor, float springConstant, float restLength)
	:anchor(anchor),
	springConstant(springConstant),
	restLength(restLength)
{
}

void ParticleAnchoredSpring::updateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition();
	force -= *anchor;

	float magnitude = force.length();
	magnitude = (restLength - magnitude) * springConstant;

	force.normalize();
	force *= magnitude;

	particle->addForce(force);
}

ParticleBungee::ParticleBungee(Particle* otherP, float springConstant, float restLength)
	:other(otherP),
	springConstant(springConstant),
	restLength(restLength)
{}

void ParticleBungee::updateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition();
	force -= other->getPosition();

	float magnitude = force.length();
	if (magnitude <= restLength)
		return;

	magnitude = springConstant * (restLength - magnitude);

	force.normalize();
	force *= magnitude;

	particle->addForce(force);
}

void ParticleForceRegistry::update(float dt)
{
	for (auto it = registrations.begin(); it != registrations.end(); ++it)
	{
		it->fg->updateForce(it->particle, dt);
	}
}

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg)
{
	ParticleForceRegistry::ParticleForceRegistration registration;
    registration.particle = particle;
    registration.fg = fg;
    registrations.push_back(registration);
}

void ParticleForceRegistry::clear()
{
	registrations.clear();
}