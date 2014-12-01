#include "Actor.h"
#include "D3DRenderer.h"

Actor::Actor()
{

}

Actor::~Actor()
{
	deleteComponents();
}

void Actor::Initialize()
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Initialize();
	}
}

void Actor::Update(float dt)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Update(dt);
	}
}
	
void Actor::PreRender(D3DRenderer* renderer)
{
	renderer->pushTransform(transform);

	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->PreRender(renderer);
	}
}

void Actor::Render(D3DRenderer* renderer)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Render(renderer);
	}

	Renderable::Render(renderer);
}

void Actor::PostRender(D3DRenderer* renderer)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->PostRender(renderer);
	}

	renderer->popTransform();
}

bool Actor::addComponent(ActorComponent* component)
{
	if (component == NULL)
		return false;

	if (component->getOwner() != NULL)//Already attached to an object
	{
		std::cout << "Could not add component; already owned by another object\n";
		return false;
	}

	ComponentId id = component->getId();

	auto it = mComponents.find(id);
	if (it != mComponents.end())
	{
		std::cout << "Component of same type already bound to object\n";
		return false;
	}

	mComponents.insert(std::make_pair(id, component));
	component->mpOwner = this;

	return true;
}

void Actor::deleteComponents()
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		delete it->second;
	}

	mComponents.clear();
}