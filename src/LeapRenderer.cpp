#include "LeapRenderer.h"
#include "LeapManager.h"
#include "d3dApp.h"

using namespace Leap;

LeapRenderer::LeapRenderer()
{
	
}

LeapRenderer::~LeapRenderer()
{

}

void LeapRenderer::Initialize()
{
	Mesh sphere;

	GeometryGenerator::CreateGeosphere(0.01f, 7, sphere);

	mMesh.Initialize(sphere.Vertices, sphere.Indices, gpApplication->getRenderer());
}

void LeapRenderer::Update()
{

	LeapManager& leapManager = LeapManager::getInstance();

	mLineRenderer.Points.clear();

	HandList hands = leapManager.getFrame()->hands();

	for (auto handit = hands.begin(); handit != hands.end(); ++handit)
	{
		const Hand hand = *handit;

		const FingerList fingers = hand.fingers();
		for (auto fingerIt = fingers.begin(); fingerIt != fingers.end(); ++fingerIt)
		{
			const Finger finger = *fingerIt;

			for (int b = 0; b < 4; b++)
			{
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				
				mLineRenderer.Points.addPoint(leapManager.transformPosition(bone.prevJoint()));
				mLineRenderer.Points.addPoint(leapManager.transformPosition(bone.nextJoint()));
			}
		}
	}

	mLineRenderer.reloadPoints();
}

void LeapRenderer::Render(D3DRenderer* renderer)
{
	mLineRenderer.Render(renderer);

	
}