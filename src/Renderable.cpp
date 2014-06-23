#include "Renderable.h"
#include "D3DRenderer.h"

void Renderable::RenderChildren(D3DRenderer* renderer)
{
	for (auto it = Children.begin(); it != Children.end(); ++it)
	{
		(*it)->PreRender(renderer);
		(*it)->Render(renderer);
		(*it)->PostRender(renderer);
	}
}