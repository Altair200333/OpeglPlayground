#pragma once
#include "OnUpdateSubscriber.h"
#include <fstream>
#include "FPSCounter.h"
#include "b3d/btBulletCollisionCommon.h"
#include "b3d/btBulletDynamicsCommon.h"

class PhysicsWorld: public OnUpdateSubscriber
{
	static PhysicsWorld& instance()
	{
		static PhysicsWorld world;
		return world;
	}
	btBroadphaseInterface* broadphase = nullptr;
	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
public:
	float timeStep = 1.0f / 60.0f;
	
	static btDiscreteDynamicsWorld& getWorld()
	{
		return *instance().dynamicsWorld;
	}
	static void init()
	{
		auto& inst = instance();
		inst.broadphase = new btDbvtBroadphase();

		inst.collisionConfiguration = new btDefaultCollisionConfiguration();
		inst.dispatcher = new btCollisionDispatcher(inst.collisionConfiguration);

		inst.solver = new btSequentialImpulseConstraintSolver;

		inst.dynamicsWorld = new btDiscreteDynamicsWorld(inst.dispatcher, inst.broadphase, inst.solver, inst.collisionConfiguration);

		inst.dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	}
	float accumulator = 0;
	void onUpdate() override
	{
		const auto deltaTime = FPSCounter::getFrameTime();
		accumulator += deltaTime;
		while (accumulator >= timeStep)
		{
			// Update the Dynamics world with a constant time step
			
			getWorld().stepSimulation(timeStep, 10);

			// Decrease the accumulated time
			accumulator -= timeStep;
		}
	}
};