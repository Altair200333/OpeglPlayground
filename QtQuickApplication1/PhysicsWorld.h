#pragma once
#include "OnUpdateSubscriber.h"
#include <fstream>
#include "FPSCounter.h"

class PhysicsWorld: public OnUpdateSubscriber
{
	static PhysicsWorld& instance()
	{
		static PhysicsWorld world;
		return world;
	}

public:
	float timeStep = 1.0f / 60.0f;
	
	
	static void init()
	{
		auto& inst = instance();
	}
	float accumulator = 0;
	void onUpdate() override
	{
		const auto deltaTime = FPSCounter::getFrameTime();
		accumulator += deltaTime;
		while (accumulator >= timeStep)
		{
			// Update the Dynamics world with a constant time step
			

			// Decrease the accumulated time
			accumulator -= timeStep;
		}
	}
};