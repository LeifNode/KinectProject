#include "ParticleRenderer.h"
#include "D3DRenderer.h"
#include "d3dApp.h"

ParticleRenderer::ParticleRenderer()
{

}

ParticleRenderer::~ParticleRenderer()
{

}

void ParticleRenderer::Initialize()
{
	Mesh sphere;

	GeometryGenerator::CreateGeosphere(0.05f, 7, sphere);

	mMeshRenderer.Initialize(sphere.Vertices, sphere.Indices, gpApplication->getRenderer());
}

void ParticleRenderer::addParticle(Particle* particle)
{
	mTargetParticles.push_back(particle);
}

void ParticleRenderer::removeParticle(Particle* particle)
{
	for (auto it = mTargetParticles.begin(); it != mTargetParticles.end(); ++it)
	{
		if ((*it) == particle)
		{
			mTargetParticles.erase(it);
			break;
		}
	}
}

void ParticleRenderer::Render(D3DRenderer* renderer)
{
	Shader* shader = renderer->getShader("Shaders/color.fx");
	renderer->setShader(shader);
	
	CBPerObject perObject;

	for (auto it = mTargetParticles.begin(); it != mTargetParticles.end(); ++it)
	{
		Vector3 pos = (*it)->getPosition();

		perObject.World = XMMatrixTranslation(pos.x, pos.y, pos.z);
		perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
		perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;
		perObject.TextureTransform = XMMatrixIdentity();

		renderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(renderer);
	}
}