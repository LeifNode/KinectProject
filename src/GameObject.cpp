#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	deleteComponents();
}

void GameObject::Initialize()
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Initialize();
	}
}

void GameObject::Update(float dt)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Update(dt);
	}
}
	
void GameObject::PreRender(D3DRenderer* renderer)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->PreRender(renderer);
	}
}

void GameObject::Render(D3DRenderer* renderer)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->Render(renderer);
	}
}

void GameObject::PostRender(D3DRenderer* renderer)
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		it->second->PostRender(renderer);
	}
}

bool GameObject::addComponent(ObjectComponent* component)
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

void GameObject::deleteComponents()
{
	auto end = mComponents.end();
	for (auto it = mComponents.begin(); it != end; ++it)
	{
		delete it->second;
	}

	mComponents.clear();
}