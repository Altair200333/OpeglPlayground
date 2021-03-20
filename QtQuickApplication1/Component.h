#pragma once

class Object;

class Component
{
protected:
	Object* owner = nullptr;
public:
	void setOwner(Object* newOwner)
	{
		owner = newOwner;
	}

	virtual ~Component() = default;
};
