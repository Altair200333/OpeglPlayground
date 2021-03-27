#pragma once
#include <memory>
#include <vector>

#include "RenderLayer.h"

class RenderLayerStack final
{
public:
	std::vector<std::shared_ptr<RenderLayer>> layers;

	void render()
	{
		for(auto& layer: layers)
		{
			layer->render();
		}
	}
};
