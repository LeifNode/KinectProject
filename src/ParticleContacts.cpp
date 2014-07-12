#include "ParticleContacts.h"

ParticleContact::ParticleContact()
{
	particle[0] = NULL;
	particle[1] = NULL;
}

void ParticleContact::resolve(float dt)
{
	resolveVelocity(dt);
	resolveInterpenetration(dt);
}

float ParticleContact::calculateSeparatingVelocity() const
{
	Vector3 reletiveVelocity = particle[0]->getVelocity();
	if (particle[1])
		reletiveVelocity -= particle[1]->getVelocity();

	return Vector3::dot(reletiveVelocity, contactNormal);
}

void ParticleContact::resolveVelocity(float dt)
{
	float separatingVelocity = calculateSeparatingVelocity();

	if (separatingVelocity > 0)
	{
		//Contact is either separating or stationary
		return;
	}

	float newSepVelocity = -separatingVelocity * restitution;

	Vector3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1])
		accCausedVelocity -= particle[1]->getAcceleration();

	float accCausedSepVelocity = Vector3::dot(accCausedVelocity, contactNormal) * dt;

	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		if (newSepVelocity < 0)
			newSepVelocity = 0;
	}

	float deltaVelocity = newSepVelocity - separatingVelocity;

	float totalInverseMass = particle[0]->getInverseMass();
	if (particle[1])
		totalInverseMass += particle[1]->getInverseMass();

	if (totalInverseMass <= 0)
		return;

	float impulse = deltaVelocity / totalInverseMass;

	Vector3 inpulsePerIMass = contactNormal * impulse;

	if (XMVectorGetX(XMVectorIsNaN(particle[0]->getPosition().mBaseVector)) != 0)
	{
		std::cout << "Is NaN\n";
	}

	particle[0]->setVelocity(particle[0]->getVelocity() + inpulsePerIMass * particle[0]->getInverseMass());
	if (particle[1])
		particle[1]->setVelocity(particle[1]->getVelocity() + inpulsePerIMass * -particle[1]->getInverseMass());
}

void ParticleContact::resolveInterpenetration(float dt)
{
	// If we don't have any penetration, skip this step.
    if (penetration <= 0) return;

    // The movement of each object is based on their inverse mass, so
    // total that.
    float totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

    // If all particles have infinite mass, then we do nothing
    if (totalInverseMass <= 0) return;

    // Find the amount of penetration resolution per unit of inverse mass
    Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);

    // Calculate the the movement amounts
    particleMovement[0] = movePerIMass * particle[0]->getInverseMass();
    if (particle[1]) {
        particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
    } else {
		particleMovement[1] = Vector3();
    }

    // Apply the penetration resolution
    particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);
    if (particle[1]) {
        particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
    }
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations)
	:iterations(iterations),
	iterationsUsed(0)
{}

void ParticleContactResolver::setIterations(unsigned iterationCount)
{
	iterations = iterationCount;
}

void ParticleContactResolver::resolveContacts(ParticleContact* contactArr, unsigned numContacts, float dt)
{
	unsigned i;

	iterationsUsed = 0;

	while (iterationsUsed < iterations)
	{
		float max = FLT_MAX;
		unsigned maxIndex = numContacts;

		for (i = 0; i < numContacts; i++)
		{
			float sepVel = contactArr[i].calculateSeparatingVelocity();
			if (sepVel < max && (sepVel < 0 || contactArr[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if (maxIndex == numContacts)
			break;

		contactArr[maxIndex].resolve(dt);

		Vector3 *move = contactArr[maxIndex].particleMovement;
        for (i = 0; i < numContacts; i++)
        {
            if (contactArr[i].particle[0] == contactArr[maxIndex].particle[0])
            {
                contactArr[i].penetration -= Vector3::dot(move[0], contactArr[i].contactNormal);
            }
            else if (contactArr[i].particle[0] == contactArr[maxIndex].particle[1])
            {
                contactArr[i].penetration -= Vector3::dot(move[1], contactArr[i].contactNormal);
            }
            if (contactArr[i].particle[1])
            {
                if (contactArr[i].particle[1] == contactArr[maxIndex].particle[0])
                {
                    contactArr[i].penetration += Vector3::dot(move[0], contactArr[i].contactNormal);
                }
                else if (contactArr[i].particle[1] == contactArr[maxIndex].particle[1])
                {
                    contactArr[i].penetration += Vector3::dot(move[1], contactArr[i].contactNormal);
                }
            }
        }

		iterationsUsed++;
	}
}