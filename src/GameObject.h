#pragma once

#include "Renderable.h"
#include "ObjectComponent.h"

class GameObject : public Renderable
{
public:
	GameObject();
	virtual ~GameObject();

	bool addComponent(ObjectComponent* component);
	//void addComponent(ComponentId id, ObjectComponent* component); //Do I want to let the id be precalculated?

	virtual void Initialize();

	virtual void Update(float dt);
	
	virtual void PreRender(D3DRenderer* renderer);
	virtual void Render(D3DRenderer* renderer);
	virtual void PostRender(D3DRenderer* renderer);

	template <class T>
	T* getComponent(ComponentId id);

	template <class T>
    T* getComponent(const char *name);

protected:
	void deleteComponents();

private:
	std::map<ComponentId, ObjectComponent*> mComponents;
};

template <class T>
T* GameObject::getComponent(ComponentId id)
{
	auto i = mComponents.find(id);
	if (i != mComponents.end())
	{
		return reinterpret_cast<T*>(i->second);
	}
	else
		return NULL;
}

template <class T>
T* GameObject::getComponent(const char *name)
{
	ComponentId id = ObjectComponent::GetIdFromName(name);
    auto i = mComponents.find(id);
    if (i != mComponents.end())
    {
		return reinterpret_cast<T*>(i->second);
    }
    else
    {
        return NULL;
    }
}