#pragma once
#include <vector>

struct HeightMap final
{
	int w, h;
	float maxValue = 2;
	float minValue = 0;
	std::vector<float> data;
	HeightMap(int _w, int _h):w(_w), h(_h)
	{
		data = std::vector<float>(w * h, 0);
	}
};
