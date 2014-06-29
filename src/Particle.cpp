#include "Particle.h"
#include <climits>

Particle::Particle()
{

}

void Particle::integrate(float dt)
{
	if (inverseMass <= 0.0f)
		return;

	assert(dt > 0.0f);

	position += velocity * dt;


	Vector3 resultingAcc = acceleration;

	resultingAcc += forceAccum * inverseMass;

	velocity += resultingAcc * dt;

	velocity *= powf(damping, dt);

	clearAccmulator();
}

void Particle::setMass(float mass)
{
	assert(mass != 0.0f);

	inverseMass = 1.0f / mass;
}

float Particle::getMass() const
{
	if (inverseMass == 0) {
		return FLT_MAX;
    } else {
        return (1.0f)/inverseMass;
    }
}

bool Particle::hasFiniteMass() const
{
    return inverseMass >= 0.0f;
}

void Particle::addForce(const Vector3& force)
{
	forceAccum += force;
}