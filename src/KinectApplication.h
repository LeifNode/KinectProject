#pragma once

#include "d3dApp.h"
#include "ConstantBuffers.h"
#include "TransformTool.h"
#include "AssetTypes.h"

template <class T>
class MeshRenderer;
struct Vertex;

namespace collada
{
	struct SceneNode;
}
class COLLADALoader;
class ParticleSystem;

class KinectApplication : public D3DApp
{
public:
	KinectApplication(HINSTANCE hInstance);
	virtual ~KinectApplication();

	void cleanup();

	bool Initialize();

	void hookInputEvents();
	void unhookInputEvents();

	void onKeyDown(IEventDataPtr);

	void onResize();
	void Update(float dt);
	void Draw();

	void onMouseDown(IEventDataPtr eventData);
	void onMouseUp(IEventDataPtr eventData);
	void onMouseMove(IEventDataPtr eventData);

private:
	void loadModels(collada::SceneNode* node, COLLADALoader* loader);
	void loadTextures(COLLADALoader* loader);
	void bindTextures(MeshRenderer<Vertex>* mesh, Material& matOut);

	void updateLines(float dt);

private:
	class PaintingSystem* mpPaintingSystem;

	TransformTool mRotationTool;
	Transform mCubeRotation;

	class Shader* mpMainShader;
	MeshRenderer<Vertex>* mpPlaneRenderer;
	std::vector<MeshRenderer<Vertex>*> mpCubeRendererArr;
	std::map<MeshRenderer<Vertex>*, assettypes::Material*> mCubeMaterials;

	class KinectRenderer* mpKinectRenderer;

	class HydraRenderer* mpHydraRenderer;

	class TextRenderer* mpText;
	class LineRenderer* mpLineRenderer;
	class LeapRenderer* mpLeapRenderer;
	class ParticleSystem* mpParticleSystem;
	class FractalRenderer* mpFractalRenderer;

	class Camera* mpCamera;

	CBPerFrame mPerFrameData;

	bool mSimluateParticles;
	bool mRenderSponza;
	float mParticleSimulationSpeed;
};