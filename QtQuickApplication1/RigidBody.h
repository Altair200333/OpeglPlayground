#pragma once

#include "ComponentManager.h"
#include "PhysicsWorld.h"
#include "Transform.h"

class RigidBody final: public Component, public OnUpdateSubscriber
{
	std::shared_ptr<Transform> transformComponent;
	btRigidBody* body = nullptr;
public:
	
	RigidBody() = default;
	
	void init(std::shared_ptr<Transform> transform, btCollisionShape* shape, float mass)
	{
		btTransform btTrans = btTransform();
		btTrans.setFromOpenGLMatrix(transform->transform.data());

		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTrans);
		
		btVector3 fallInertia(0, 0, 0);
		shape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo bodyCI(1, fallMotionState, shape, fallInertia);

		body = new btRigidBody(bodyCI);

		
		PhysicsWorld::getWorld().addRigidBody(body);
	}
	void addCollider()
	{

	}
	void onUpdate() override
	{
		if (body == nullptr)
			return;
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);

		auto tr = ComponentManager::getComponent<Transform>(owner);
		trans.getOpenGLMatrix(tr->transform.data());
	}
};
