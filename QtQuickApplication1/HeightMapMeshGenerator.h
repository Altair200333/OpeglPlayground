#pragma once
#include <memory>
#include "HeightMap.h"
#include "Mesh.h"

class HeightMapMeshGenerator final
{
public:
	std::shared_ptr<Mesh> genMesh(std::shared_ptr<HeightMap> map)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		int vertexId = 0;
		for(int i=0;i<map->w-1;++i)
		{
			for (int j = 0; j < map->h-1; ++j)
			{
				auto triangle1 = genTriangle(map, i, j, i + 1, j + 1, i + 1, j);
				for(auto& v: triangle1)
				{
					mesh->vertices.push_back(v);
					mesh->indices.push_back(vertexId++);
				}
				auto triangle2 = genTriangle(map, i, j, i, j+1, i + 1, j + 1);
				for (auto& v : triangle2)
				{
					mesh->vertices.push_back(v);
					mesh->indices.push_back(vertexId++);
				}
			}
		}
		return mesh;
	}
	std::array<Vertex, 3> genTriangle(const std::shared_ptr<HeightMap>& map, int x1, int y1, int x2, int y2, int x3, int y3)
	{
		std::array<Vertex, 3> polygon;
		polygon[0] = getVertexAt(map, x1, y1);
		polygon[1] = getVertexAt(map, x2, y2);
		polygon[2] = getVertexAt(map, x3, y3);
		const QVector3D d1{ polygon[0].vertex[0] - polygon[1].vertex[0],polygon[0].vertex[1] - polygon[1].vertex[1],polygon[0].vertex[2] - polygon[1].vertex[2]};
		const QVector3D d2{ polygon[0].vertex[0] - polygon[2].vertex[0],polygon[0].vertex[1] - polygon[2].vertex[1],polygon[0].vertex[2] - polygon[2].vertex[2]};
		const QVector3D normal = QVector3D::crossProduct(d1, d2).normalized();
		
		for (auto& v : polygon)
		{
			v.normal = {normal.x(),normal.y(),normal.z()};
		}
		return polygon;
	}
	Vertex getVertexAt(const std::shared_ptr<HeightMap>& map, int i, int j)
	{
		const float originHeight = -(map->maxValue - map->minValue) * 0.5f - map->minValue;
		const float height = originHeight + map->data[j * map->h + i];

		Vertex vertex{};
		vertex.vertex = std::array<float, 3>{ -(map->w - 1) * 0.5f + i, height, -(map->h - 1) * 0.5f + j};
		vertex.normal = std::array<float, 3>{0, 1, 0};
		vertex.Tangent = std::array<float, 3>{1, 0, 0};
		vertex.Bitangent = std::array<float, 3>{0, 0, 1};
		vertex.TexCoords = std::array<float, 2>{0, 0};

		return vertex;
	}
};
