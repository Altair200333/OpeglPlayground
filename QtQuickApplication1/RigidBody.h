#pragma once
#include <reactphysics3d/reactphysics3d.h>

#include "ComponentManager.h"
#include "PhysicsWorld.h"
#include "Transform.h"

class RigidBody final: public Component, public OnUpdateSubscriber
{
	std::shared_ptr<Transform> transformComponent;
public:
	using Type = reactphysics3d::BodyType;
	reactphysics3d::RigidBody* body = nullptr;
	reactphysics3d::Collider* collider = nullptr;
	
	Type bodyType;
	
	RigidBody() = default;
	
	void init(Type _bodyType = Type::DYNAMIC)
	{
		transformComponent = ComponentManager::getComponent<Transform>(owner);
		reactphysics3d::Transform transform;

		float data[16];
		transformComponent->transform.copyDataTo(data);//row-major order
		transform.setFromOpenGL(data);
		transform.setPosition(reactphysics3d::Vector3(transformComponent->position.x(), transformComponent->position.y(), transformComponent->position.z()));
		
		body = PhysicsWorld::getWorld().createRigidBody(transform);//why???
		bodyType = _bodyType;
		body->setType(bodyType);
	}
	void setCollider(reactphysics3d::CollisionShape* shape, reactphysics3d::Transform transform)
	{
		collider = body->addCollider(shape, transform);
	}
	void onUpdate() override
	{
		if(body == nullptr)
			return;
		if(bodyType == Type::DYNAMIC)
		{
			const auto& transform = body->getTransform();
			const auto& pos = transform.getPosition();
			transformComponent->position = QVector3D(pos.x, pos.y, pos.z);

			transform.getOpenGLMatrix(transformComponent->transform.data());

			transformComponent->right = transformComponent->transform * QVector3D(1, 0, 0) - transformComponent->position;
			transformComponent->up = transformComponent->transform * QVector3D(0, 1, 0) - transformComponent->position;
			transformComponent->forward = transformComponent->transform * QVector3D(0, 0, 1) - transformComponent->position;
		}
		else if(bodyType == Type::KINEMATIC || bodyType == Type::STATIC)
		{
			reactphysics3d::Transform transform;

			float data[16];
			transformComponent->transform.copyDataTo(data);//row-major order
			transform.setFromOpenGL(data);
			auto& pos = transformComponent->position;
			transform.setPosition(reactphysics3d::Vector3(pos.x(), pos.y(), pos.z()));
			body->setTransform(transform.getInverse());
		}
	}
};
