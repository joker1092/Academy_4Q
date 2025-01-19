#pragma once
#include "Utility_Framework/IComponent.h"

class Object;
class Component : public IComponent
{
public:
	Component() = default;
	virtual ~Component() = default;
	virtual void Update() = 0;
	void SetOwner(Object* owner) {_owner = owner; }
	Object* GetOwner() const { return _owner; }
private:
	Object* _owner;
};

