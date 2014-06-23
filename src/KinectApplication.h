#pragma once

#include "d3dApp.h"
#include "ConstantBuffers.h"
#include "TransformTool.h"

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
	TransformTool mRotationTool;
	Transform mCubeRotation;

	OVRRenderer* mpOVRRenderer;

	Shader* mpMainShader;
	MeshRenderer<Vertex>* mpPlaneRenderer;
	MeshRenderer<Vertex>* mpCubeRenderer;

	KinectRenderer* mpKinectRenderer;

	HydraRenderer* mpHydraRenderer;

	TextRenderer* mpText;
	LineRenderer* mpLineRenderer;
	LeapRenderer* mpLeapRenderer;

	Camera* mpCamera;

	CBPerFrame mPerFrameData;
};