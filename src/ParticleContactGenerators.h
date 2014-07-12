#pragma once

#include "d3dStd.h"
#include "Vector3.h"
#include "Particle.h"
#include "ParticleContacts.h"

class ParticleContactGenerator
{
public:
	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
};

class ParticleLink : public ParticleContactGenerator
{
public:
	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;

public:
	Particle* particle[2];

protected:
	float currentLength() const;
};

class ParticleCable : public ParticleLink
{
public:
	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;

public:
	float maxLength;

	float restitution;
};

class ParticleRod : public ParticleLink
{
public:
	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;

public:
	float length;
};

class ParticlePlane : public ParticleContactGenerator
{
public:
	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;

public:
	std::vector<Particle*>* pParticles;

	float restitution;
};