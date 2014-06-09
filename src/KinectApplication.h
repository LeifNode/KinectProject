#pragma once

#include "d3dApp.h"
#include "ConstantBuffers.h"

class OVRRenderer;
class KinectRenderer;
class Shader;
class Camera;
class HydraManager;

template <class T>
class MeshRenderer;
struct Vertex;

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

	OVRRenderer* mpOVRRenderer;

	Shader* mpMainShader;
	MeshRenderer<Vertex>* mpMeshRenderer;

	KinectRenderer* mpKinectRenderer;

	HydraManager* mpHydraManager;

	Camera* mpCamera;

	CBPerFrame mPerFrameData;
};