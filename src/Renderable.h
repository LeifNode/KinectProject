#pragma once

#include "d3dStd.h"

class D3DRenderer;

class Renderable 
{
public:
	Renderable();
	virtual ~Renderable();

	virtual void PreRender(D3DRenderer* renderer) {}
	virtual void Render(D3DRenderer* renderer) {}
	virtual void PostRender(D3DRenderer* renderer) {}

	void RenderChildren(D3DRenderer* renderer);

public:
	std::list<Renderable*> Children;

protected:
private:
};