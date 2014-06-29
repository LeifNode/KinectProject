#include "ConstraintRenderer.h"
#include "Particle.h"

using namespace std;

ConstraintRenderer::ConstraintRenderer(float lineThickness, const XMFLOAT4& color)
{
	mLineRenderer.setLineThickness(lineThickness);
	mLineRenderer.setLineColor(color);
}

ConstraintRenderer::~ConstraintRenderer()
{
}

void ConstraintRenderer::addConstraint(Particle* p1, Particle* p2)
{
	mParticleReferences.push_back(std::make_pair(p1, p2));
}

void ConstraintRenderer::removeConstraint(Particle* p1, Particle* p2)
{
	for (auto it = mParticleReferences.begin(); it != mParticleReferences.end(); ++it)
	{
		if ((it->first == p1 && it->second == p2) ||
			(it->first == p2 && it->second == p1))
		{
			mParticleReferences.erase(it);

			return;
		}
	}
}

void ConstraintRenderer::removeConnectedConstraints(Particle* particle)
{
	vector<list<pair<Particle*, Particle*>>::iterator> removedConstraints;

	for (auto it = mParticleReferences.begin(); it != mParticleReferences.end(); ++it)
	{
		if (it->first == particle || it->second == particle)
			removedConstraints.push_back(it);
	}

	for (auto it = removedConstraints.begin(); it != removedConstraints.end(); ++it)
	{
		mParticleReferences.erase((*it));
	}
}

void ConstraintRenderer::Update(float dt)
{
	mLineRenderer.Points.clear();

	for (auto it = mParticleReferences.begin(); it != mParticleReferences.end(); ++it)
	{
		mLineRenderer.Points.addPoint(it->first->getPosition().mBaseVector);
		mLineRenderer.Points.addPoint(it->second->getPosition().mBaseVector);
	}

	mLineRenderer.reloadPoints();
}

void ConstraintRenderer::Render(D3DRenderer* renderer)
{
	mLineRenderer.Render(renderer);
}