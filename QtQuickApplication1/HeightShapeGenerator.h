#pragma once
#include "CollisionShapeGenerator.h"
#include "HeightMap.h"
#include "PhysicsWorld.h"
class HeightShapeGenerator: public CollisionShapeGenerator
{
	std::shared_ptr<HeightMap> map;
public:
	HeightShapeGenerator(std::shared_ptr<HeightMap> _map):map(_map)
	{
	}
	reactphysics3d::CollisionShape* generate() override
	{
		const int nbRows = map->w;
		const int nbColumns = map->h;
		
		rp3d::HeightFieldShape* heightFieldShape = PhysicsWorld::getCommon().createHeightFieldShape(nbColumns,
			nbRows, 0,
			map->maxValue, map->data.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
		return heightFieldShape;
	}
};