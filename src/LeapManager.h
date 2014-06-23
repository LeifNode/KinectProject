#pragma once

#include "d3dStd.h"
#include <leap\Leap.h>

class LeapManager : public Leap::Listener
{
public:
	static LeapManager& getInstance()
    {
        static LeapManager instance;

        return instance;
    }

	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onExit(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    virtual void onFocusGained(const Leap::Controller&);
    virtual void onFocusLost(const Leap::Controller&);
	virtual void onDeviceChange(const Leap::Controller&);
    virtual void onServiceConnect(const Leap::Controller&);
    virtual void onServiceDisconnect(const Leap::Controller&);

	void Initialize();
	void DeInit();

	void Update(float dt);

	void setRotation(const XMVECTOR& rotationQuat);
	void setViewTransform(const XMMATRIX& mat);

	const Leap::Frame* getFrame() const { return &mFrame; }
	
	XMVECTOR transformPosition(const Leap::Vector&);
	XMVECTOR transformRotation(const Leap::Vector&);

private:
	LeapManager();
	~LeapManager();

	Leap::Frame mFrame;

	Leap::Controller mController;

	XMMATRIX mInverseTransform;
};