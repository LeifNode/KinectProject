#pragma once

#include "d3dApp.h"
#include "ConstantBuffers.h"

class Shader;

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
	Shader* mpMainShader;
	MeshRenderer<Vertex>* mpMeshRenderer;

	float mTheta, mPhi, mRadius;

	POINT mLastMousePos;

	float currentRot;

	CBPerFrame mPerFrameData;
};