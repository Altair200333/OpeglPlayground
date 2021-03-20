#pragma once
#include "MeshRenderer.h"

class MeshRendererGenerator
{
public:
	virtual ~MeshRendererGenerator() = default;
	virtual std::shared_ptr<MeshRenderer> getRenderer() = 0;
};
