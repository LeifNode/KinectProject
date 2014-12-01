#pragma once

#include "Renderable.h"
#include "ActorComponent.h"

class Actor : public Renderable
{
public:
	Actor();
	virtual ~Actor();

	virtual void Destroy()=0;

	bool addComponent(ActorComponent* component);
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
	std::map<ComponentId, ActorComponent*> mComponents;
};

template <class T>
T* Actor::getComponent(ComponentId id)
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
T* Actor::getComponent(const char *name)
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