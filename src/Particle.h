#pragma once

#include "Vector3.h"

class Particle
{
	friend class PhysicsSystem;

public:
	Particle();

	void integrate(float dt);

	void setVelocity(const Vector3& newVel) { velocity = newVel; }
	Vector3 getVelocity() const { return velocity; }
	Vector3 getAcceleration() const { return acceleration; }
	Vector3 getPosition() const { return position; }

	void setMass(float mass);
	float getMass() const;

	float getInverseMass() const;

	bool hasFiniteMass() const;

	void setDamping(float newDamping) { damping = newDamping; }
	float getDamping() const { return damping; }

	void setPosition(const Vector3& pos) { position = pos; }

	void clearAccmulator() { forceAccum = Vector3(); }

	void addForce(const Vector3& force);

protected:
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 forceAccum;

	float damping;

	float inverseMass;
private:
};