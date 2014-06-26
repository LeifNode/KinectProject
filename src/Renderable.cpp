#include "Renderable.h"
#include "D3DRenderer.h"

void Renderable::renderChildren(D3DRenderer* renderer)
{
	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		(*it)->PreRender(renderer);
		(*it)->Render(renderer);
		(*it)->PostRender(renderer);
	}
}

void Renderable::deleteChildren()
{
	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		delete (*it);
	}

	mChildren.clear();
}