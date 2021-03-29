#pragma once
#include <reactphysics3d/reactphysics3d.h>

#include "ComponentManager.h"
#include "PhysicsWorld.h"
#include "Transform.h"

class RigidBody final: public Component, public OnUpdateSubscriber
{
	std::shared_ptr<Transform> transformComponent;
public:
	reactphysics3d::RigidBody* body;
	RigidBody() = default;
	
	void init()
	{
		transformComponent = ComponentManager::getComponent<Transform>(owner);
		reactphysics3d::Transform transform;

		float data[16];
		transformComponent->transform.copyDataTo(data);//row-major order
		transform.setFromOpenGL(data);
		
		body = PhysicsWorld::getWorld().createRigidBody(transform);
	}

	void onUpdate() override
	{
		if(body == nullptr)
			return;
		
		const auto& transform = body->getTransform();
		transform.getOpenGLMatrix(transformComponent->transform.data());
	}
};
