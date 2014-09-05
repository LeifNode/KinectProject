#pragma once

#include "d3dApp.h"
#include "ConstantBuffers.h"
#include "TransformTool.h"
#include "AssetTypes.h"

class OVRRenderer;
class KinectRenderer;
class Shader;
class Camera;
class HydraRenderer;

template <class T>
class MeshRenderer;
struct Vertex;
class TextRenderer;
class LineRenderer;
class LeapRenderer;
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

private:
	TransformTool mRotationTool;
	Transform mCubeRotation;

	OVRRenderer* mpOVRRenderer;

	Shader* mpMainShader;
	MeshRenderer<Vertex>* mpPlaneRenderer;
	std::vector<MeshRenderer<Vertex>*> mpCubeRendererArr;
	std::map<MeshRenderer<Vertex>*, assettypes::Material*> mCubeMaterials;

	KinectRenderer* mpKinectRenderer;

	HydraRenderer* mpHydraRenderer;

	TextRenderer* mpText;
	LineRenderer* mpLineRenderer;
	LeapRenderer* mpLeapRenderer;
	ParticleSystem* mpParticleSystem;

	Camera* mpCamera;

	CBPerFrame mPerFrameData;
};