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
		
		if(mass!=0)
			shape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, fallMotionState, shape, fallInertia);

		body = new btRigidBody(bodyCI);
		body->setDamping(0.02, 0.1);
		PhysicsWorld::getWorld().addRigidBody(body);
	}
	auto velocity() const
	{
		return body->getLinearVelocity();
	}
	auto qvelocity() const
	{
		auto& v = body->getLinearVelocity();
		
		return QVector3D(v.x(), v.y(), v.z());
	}
	void init(btCollisionShape* shape, float mass)
	{
		init(ComponentManager::getComponent<Transform>(owner), shape, mass);
	}
	void setAngularDamping(float damping)
	{
		body->setDamping(body->getLinearDamping(), damping);
	}
	void setLinearDamping(float damping)
	{
		body->setDamping(damping, body->getAngularDamping());
	}
	void setGravity(const QVector3D& gravity)
	{
		body->setGravity(btVector3(gravity.x(), gravity.y(), gravity.z()));
	}
	void addCollider()
	{
	}
	void addForce(const QVector3D& force)
	{
		body->activate(true);//wake up samurai we've got a force to add
		body->applyCentralForce(btVector3(force.x(), force.y(), force.z()));
	}
	void addTorgue(const QVector3D& torgue)
	{
		body->activate(true);
		body->applyTorque(btVector3(torgue.x(), torgue.y(), torgue.z()));
	}
	void onUpdate() override
	{
		if (body == nullptr)
			return;
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);

		auto tr = ComponentManager::getComponent<Transform>(owner);
		trans.getOpenGLMatrix(tr->transform.data());
		
		auto pos = trans.getOrigin();
		tr->position = QVector3D(pos.x(), pos.y(), pos.z());
	}
};
