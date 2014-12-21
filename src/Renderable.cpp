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

void Renderable::PreRender(D3DRenderer* renderer)
{
	renderer->pushTransform(transform);
}

void Renderable::Render(D3DRenderer* renderer)
{
	renderChildren(renderer);
}

void Renderable::PostRender(D3DRenderer* renderer)
{
	renderer->popTransform();
}

void Renderable::addChild(Renderable* child)
{
	if (child->mpParent == this)
		return;
	else if (child->mpParent != NULL)
		child->mpParent->removeChild(child);

	mChildren.push_back(child);

	child->mpParent = this;
}

void Renderable::removeChild(Renderable* child)
{
	if (child->mpParent != this)
		return;

	mChildren.remove(child);

	child->mpParent = NULL;
}