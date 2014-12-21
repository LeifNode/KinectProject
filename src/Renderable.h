#pragma once

#include "d3dStd.h"
#include "Transform.h"

class D3DRenderer;

class Renderable 
{
public:
	Renderable() : mRenderEnabled(true), mpParent(NULL) { }
	virtual ~Renderable() { }

	virtual void PreRender(D3DRenderer* renderer);
	virtual void Render(D3DRenderer* renderer);
	virtual void PostRender(D3DRenderer* renderer);

	std::list<Renderable*>* getChildren() { return &mChildren; }

	void addChild(Renderable* child);
	void removeChild(Renderable* child);

public:
	Transform transform;
	
protected:
	void deleteChildren();
	void renderChildren(D3DRenderer* renderer);

protected:
	Renderable* mpParent;

	std::list<Renderable*> mChildren;

private:
	bool mRenderEnabled;
};