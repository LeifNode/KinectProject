#include "ParticleContactGenerators.h"

float ParticleLink::currentLength() const
{
	Vector3 relativePos = particle[0]->getPosition() - particle[1]->getPosition();

	return relativePos.length();
}

unsigned ParticleCable::addContact(ParticleContact* contact, unsigned limit) const
{
	float length = currentLength();

	if (length < maxLength)
	{
		return 0;
	}

	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	Vector3 normal = particle[1]->getPosition() - particle[0]->getPosition();
	normal.normalize();

	contact->contactNormal = normal;

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned ParticleRod::addContact(ParticleContact* contact, unsigned limit) const
{
	float currentLen = currentLength();

	if (currentLen == length)
		return 0;

	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	Vector3 normal = particle[1]->getPosition() - particle[0]->getPosition();
	normal.normalize();

	if (currentLen > length)
	{
		contact->contactNormal = normal;
		contact->penetration = currentLen - length;
	}
	else
	{
		contact->contactNormal = -normal;
		contact->penetration = length - currentLen;
	}

	contact->restitution = 0;

	return 1;
}

unsigned ParticlePlane::addContact(ParticleContact* contact, unsigned limit) const
{
	unsigned count = 0;
	for (auto p = pParticles->begin();
        p != pParticles->end();
        p++)
    {
        float y = (*p)->getPosition().y;
        if (y < 0.0f)
        {
            contact->contactNormal = Vector3(0.0f, 1.0f, 0.0f);
            contact->particle[0] = *p;
            contact->particle[1] = NULL;
            contact->penetration = -y;
			contact->restitution = restitution;
            contact++;
            count++;
        }

        if (count >= limit) return count;
    }

    return count;
}