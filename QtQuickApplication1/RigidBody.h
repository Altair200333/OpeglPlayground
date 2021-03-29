#pragma once
#include <reactphysics3d/reactphysics3d.h>

#include "ComponentManager.h"
#include "PhysicsWorld.h"
#include "Transform.h"

class RigidBody final: public Component, public OnUpdateSubscriber
{
	std::shared_ptr<Transform> transformComponent;
public:
	reactphysics3d::RigidBody* body = nullptr;
	
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
		const auto& pos = transform.getPosition();
		transformComponent->position = QVector3D(pos.x, pos.y, pos.z);
		
		transform.getOpenGLMatrix(transformComponent->transform.data());
		
		transformComponent->right = transformComponent->transform * QVector3D(1, 0, 0) - transformComponent->position;
		transformComponent->up = transformComponent->transform * QVector3D(0, 1, 0) - transformComponent->position;
		transformComponent->forward = transformComponent->transform * QVector3D(0, 0, 1) - transformComponent->position;
	}
};
