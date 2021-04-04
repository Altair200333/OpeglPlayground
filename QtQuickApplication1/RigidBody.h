#pragma once

#include "ComponentManager.h"
#include "PhysicsWorld.h"
#include "Transform.h"

class RigidBody final: public Component, public OnUpdateSubscriber
{
	std::shared_ptr<Transform> transformComponent;
public:
	
	RigidBody() = default;
	
	void init()
	{
		
	}
	void addCollider()
	{

	}
	void onUpdate() override
	{
	}
};
