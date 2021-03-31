#pragma once
#include <QVector3D>

#include "CollisionShapeGenerator.h"
#include "PhysicsWorld.h"

class BoxShapeGenerator: public CollisionShapeGenerator
{
public:
	QVector3D halfSize;
	BoxShapeGenerator(QVector3D halfExtent)
	{
		halfSize = halfExtent;
	}
	BoxShapeGenerator(reactphysics3d::Vector3 halfExtent)
	{
		halfSize = QVector3D(halfExtent.x, halfExtent.y, halfExtent.z);
	}
	reactphysics3d::CollisionShape* generate() override
	{
		reactphysics3d::BoxShape* boxShape = PhysicsWorld::getCommon().createBoxShape(reactphysics3d::Vector3(halfSize.x(), halfSize.y(), halfSize.z()));
		return boxShape;
	}
};
