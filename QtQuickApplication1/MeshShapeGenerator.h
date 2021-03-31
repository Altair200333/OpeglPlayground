#pragma once
#include <QVector3D>

#include "CollisionShapeGenerator.h"
#include "MeshLoader.h"
#include "PhysicsWorld.h"

class MeshShapeGenerator : public CollisionShapeGenerator
{
public:
	std::string path;
	MeshShapeGenerator(const std::string& _path)
	{
		path = _path;
	}

	reactphysics3d::CollisionShape* generate() override
	{
		auto model = MeshLoader().loadModel(path);
		auto mesh = model[0].mesh;
		
		
		int faces = mesh->vertices.size() / 3;
		
		auto polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[faces];
		rp3d::PolygonVertexArray::PolygonFace* face = polygonFaces;

		std::vector<rp3d::Vector3>* vertices = new std::vector<rp3d::Vector3>();
		std::vector<int>* indices = new std::vector<int>();
		for (int f = 0; f < faces; f++)
		{
			for (int v = 0; v < 3; v++) 
			{
				auto& vert = mesh->vertices[f * 3 + v].vertex;
				const rp3d::Vector3 vertex = rp3d::Vector3(vert[0], vert[1], vert[2]);
				
				int vIndex = findVertexIndex(*vertices, vertex);
				if (vIndex == -1) 
				{
					vIndex = vertices->size();
					vertices->push_back(vertex);
				}

				indices->push_back(vIndex);
			}
			// First vertex of the face in the indices array
			face->indexBase = f * 3;
			// Number of vertices in the face
			face->nbVertices = 3;
			face++;
		}
		rp3d::PolygonVertexArray* polygonVertexArray = new
			rp3d::PolygonVertexArray(vertices->size(), vertices->data(), sizeof(rp3d::Vector3), 
				indices->data(), sizeof(int), faces,
				polygonFaces, rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
		//Create the polyhedron mesh
		reactphysics3d::PolyhedronMesh* polyhedronMesh = PhysicsWorld::getCommon().createPolyhedronMesh(polygonVertexArray);
		// Create the convex mesh collision shape
		reactphysics3d::ConvexMeshShape* convexMeshShape = PhysicsWorld::getCommon().createConvexMeshShape(polyhedronMesh);
		
		return convexMeshShape;
	}
	int findVertexIndex(const std::vector<rp3d::Vector3>& vertices, const rp3d::Vector3& vertex) {

		for (int i = 0; i < vertices.size(); i++) {
			if (vertices[i] == vertex) {
				return i;
			}
		}

		return -1;
	}
};