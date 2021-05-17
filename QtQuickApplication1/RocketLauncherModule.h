#pragma once
#include <algorithm>
#include <memory>


#include "CollisionShapeGenerator.h"
#include "Module.h"
#include "Object.h"
#include "RigidBody.h"
#include "Scene.h"
#include "FPSCounter.h"

class RocketLauncherModule : public Module
{
	std::shared_ptr<RigidBody> body;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Object> parent;

	struct Rocket
	{
		std::shared_ptr<Object> rocket;
		std::shared_ptr<Transform> target = nullptr;
		bool launched = false;
	};

	Rocket rockets[4];
	int active = 0;
	float rocketX[4] = { -1.93016, -1.35912, 1.35912, 1.93016};
public:
	void init(Scene* scene, std::shared_ptr<RigidBody> _body, std::shared_ptr<Transform> _transform,
	          std::shared_ptr<Object> _parent)
	{
		body = _body;
		transform = _transform;
		parent = _parent;

		for (int i = 0; i < 4; ++i)
		{
			loadPlanePart(scene, "Assets/Models/plane/rocket.obj", ShaderCollection::shaders["normals"],
			              { rocketX[i], -0.339718, 0.74305});
			rockets[i].rocket = scene->objects.back();
		}
	}

	void loadPlanePart(Scene* scene, const std::string& path, ShaderData data = ShaderCollection::shaders["normals"],
	                   const QVector3D& pos = QVector3D(0, 0, 0))
	{
		scene->addModel(MeshLoader().loadModel(path), pos, data);
		parent->addChild(scene->objects.back());
	}

	void update() override
	{
		if (Input::keyJustPressed(Qt::Key_Space) && active>=0 && active<4)
		{
			if (!rockets[active].launched)
			{
				auto tr = ComponentManager::getComponent<Transform>(rockets[active].rocket);
				tr->transform = tr->getGlobalTransform();

				parent->removeChild(rockets[active].rocket);

				ComponentManager::addComponent(rockets[active].rocket, std::make_shared<RigidBody>())->init(
					CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/rocket_c.obj"), 1);
				auto rb = ComponentManager::getComponent<RigidBody>(rockets[active].rocket);
				rb->setLinearDamping(0.4);
				rb->setAngularDamping(0.5);
				rb->setGravity(QVector3D(0, -1, 0));
				rockets[active].launched = true;
				active++;
			}
		}
		for (auto& rocket : rockets)
		{
			
			if (rocket.launched)
			{
				auto tr = ComponentManager::getComponent<Transform>(rocket.rocket);
				ComponentManager::getComponent<RigidBody>(rocket.rocket)->addForce(tr->getForward() * 100);
			}
		}
	}
};
