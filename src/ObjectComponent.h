#pragma once

#include "d3dStd.h"
#include "StringUtils.h"

typedef unsigned int ComponentId;
class GameObject;
class D3DRenderer;

class ObjectComponent
{
	friend class GameObject;

public:
	ObjectComponent() : mpOwner(NULL) { }
	virtual ~ObjectComponent() { mpOwner = NULL; }

	virtual void Initialize() { }
	
	virtual void Update(float dt) { }

	virtual void PreRender(D3DRenderer* renderer) { }
	virtual void Render(D3DRenderer* renderer) { }
	virtual void PostRender(D3DRenderer* renderer) { }

	virtual ComponentId getId(void) const { return getIdFromName(getName()); }
	virtual const char *getName() const=0;
	static ComponentId getIdFromName(const char* componentStr)
	{
		void* rawId = HashedString::hash_name(componentStr);
		return reinterpret_cast<ComponentId>(rawId);
	}

	GameObject* getOwner() const { return mpOwner; }

private:
	GameObject* mpOwner;
};