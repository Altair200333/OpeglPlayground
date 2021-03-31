#pragma once
#include <memory>

#include "HeightMap.h"

class HeightMapGenerator final
{
public:
	std::shared_ptr<HeightMap> genHeightMap(int w, int h, float maxH, int seed = 42)
	{
		std::shared_ptr<HeightMap> map = std::make_shared<HeightMap>(w, h);
		map->maxValue = maxH;
		for (int i = 0; i < map->w; ++i)
		{
			for (int j = 0; j < map->h; ++j)
			{
				map->data[i * map->h + j] = maxH*static_cast<float>(i) / map->w;
			}
		}
		return map;
	}
};