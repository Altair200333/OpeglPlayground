#pragma once
#include <memory>


#include "FastNoiseLite.h"
#include "HeightMap.h"

class HeightMapGenerator final
{
public:
	std::shared_ptr<HeightMap> genHeightMap(int w, int h, float minH, float maxH, int seed = 42)
	{
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> distribution(minH, maxH);
		std::shared_ptr<HeightMap> map = std::make_shared<HeightMap>(w, h);
		map->maxValue = maxH;
		map->minValue = minH;

		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

		for (int i = 0; i < map->w; ++i)
		{
			for (int j = 0; j < map->h; ++j)
			{
				map->data[i * map->h + j] = (noise.GetNoise(static_cast<float>(i), static_cast<float>(j))+1)/2*(maxH-minH)-minH;//distribution(generator);
			}
		}
		return map;
	}
};