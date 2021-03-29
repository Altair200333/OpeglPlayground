#pragma once
#include "OnUpdateSubscriber.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsWorld: public OnUpdateSubscriber
{
	static PhysicsWorld& instance()
	{
		static PhysicsWorld world;
		return world;
	}

	reactphysics3d::PhysicsWorld* world = nullptr;
	reactphysics3d::PhysicsCommon* physicsCommon = nullptr;
public:
	reactphysics3d::decimal timeStep = 1.0f / 120.0f;
	
	static reactphysics3d::PhysicsWorld& getWorld()
	{
		return *instance().world;
	}
	static reactphysics3d::PhysicsCommon& getCommon()
	{
		return *instance().physicsCommon;
	}
	static void init()
	{
		auto& inst = instance();
		inst.physicsCommon = new reactphysics3d::PhysicsCommon();
		inst.world = inst.physicsCommon->createPhysicsWorld();
	}

	void onUpdate() override
	{
		world->update(timeStep);
	}
};