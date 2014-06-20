#pragma once

#include "d3dStd.h"

class D3DRenderer;

class Renderable 
{
public:
	Renderable();
	virtual ~Renderable();

	virtual void Render(D3DRenderer* renderer);

public:
	std::list<Renderable*> Children;

protected:
private:
};