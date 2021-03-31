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
		for(int i=0;i<map->w;++i)
		{
			for (int j = 0; j < map->h; ++j)
			{
				float originHeight = -(map->maxValue - 0) * 0.5f - 0;
				float height = originHeight + map->data[j * map->h + i];
				
				Vertex vertex{};
				vertex.vertex = std::array<float, 3>{ -(map->w - 1) * 0.5f + i, height, -(map->h - 1) * 0.5f + j};
				vertex.normal = std::array<float, 3>{0, 1, 0};
				vertex.Tangent = std::array<float, 3>{1, 0, 0};
				vertex.Bitangent = std::array<float, 3>{0, 0, 1};
				vertex.TexCoords = std::array<float, 2>{0,0};
				mesh->vertices.push_back(vertex);
				if ((i <  map->w - 1) && (j < map->h - 1))
				{

					unsigned int v1 = vertexId;
					unsigned int v2 = vertexId + 1;
					unsigned int v3 = vertexId + map->h;
					unsigned int v4 = vertexId + map->h + 1;

					// First triangle
					mesh->indices.push_back(v1);
					mesh->indices.push_back(v2);
					mesh->indices.push_back(v3);

					// Second triangle
					mesh->indices.push_back(v2);
					mesh->indices.push_back(v4);
					mesh->indices.push_back(v3);
				}

				vertexId++;
			}
		}
		return mesh;
	}
};
