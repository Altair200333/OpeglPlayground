#pragma once
#include <QVector3D>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include "b3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "b3d/BulletCollision/CollisionShapes/btConvexHullShape.h"

#include "HeightMap.h"

class CollisionShapeGenerator final
{
public:
	static btCollisionShape* getBox(const QVector3D& size)
	{
		return new btBoxShape(btVector3(size.x(), size.y(), size.z()));
	}
	static btCollisionShape* getPlane(const QVector3D& normal)
	{
		return new btStaticPlaneShape(btVector3(normal.x(), normal.y(), normal.z()), 0);
	}
	static btCollisionShape* getTerrain(std::shared_ptr<HeightMap> map)
	{
		auto shape = new btHeightfieldTerrainShape(map->w, map->h, map->data.data(),
			1, map->minValue, map->maxValue, 1, PHY_FLOAT, false);
		//auto tmap = shape->getTriangleInfoMap();
		//btBvhTriangleMeshShape* tshape = new btBvhTriangleMeshShape(tmap);
		shape->setUseDiamondSubdivision(true);
		return shape;
	}
	static btCollisionShape* getMeshCollider(const std::string& path)
	{
		auto model = MeshLoader().loadModel(path);
		auto mesh = model[0].mesh;
		btConvexHullShape* shape = new btConvexHullShape(reinterpret_cast<btScalar*>(mesh->vertices.data()), mesh->vertices.size(), sizeof(Vertex));
		shape->optimizeConvexHull();
		return shape;
	}
	static btCollisionShape* getCompoundShape(std::initializer_list<btCollisionShape*> list)
	{
		btCompoundShape* compoundShape = new btCompoundShape();
		
		for(btCollisionShape* shape: list)
		{
			compoundShape->addChildShape(btTransform::getIdentity(), shape);
		}
		return compoundShape;
	}
};
