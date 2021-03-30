#pragma once
#include "OnUpdateSubscriber.h"
#include "reactphysics3d/reactphysics3d.h"
#include <fstream>

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
	reactphysics3d::decimal timeStep = 1.0f / 60.0f;
	
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
	float accumulator = 0;
	void onUpdate() override
	{
		const auto deltaTime = FPSCounter::getFrameTime();
		accumulator += deltaTime;
		while (accumulator >= timeStep)
		{
			// Update the Dynamics world with a constant time step
			world->update(timeStep);
			// Decrease the accumulated time
			accumulator -= timeStep;
		}
	}
};