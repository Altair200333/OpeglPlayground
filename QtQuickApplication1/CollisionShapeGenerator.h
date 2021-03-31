#pragma once
#include <reactphysics3d/reactphysics3d.h>

class CollisionShapeGenerator
{
public:
	virtual reactphysics3d::CollisionShape* generate() = 0;
	
	virtual ~CollisionShapeGenerator() = default;
};