#pragma once
#include "GLBackground.h"
#include "GLCamera.h"
#include "LightSource.h"

struct RenderContext
{
	std::shared_ptr<GLCamera> camera;
	const std::vector<std::shared_ptr<LightSource>>& lights = std::vector<std::shared_ptr<LightSource>>{};
	std::shared_ptr<GLBackground> background = nullptr;
	unsigned int shadowMap = 0;
	QMatrix4x4* lightSpaceMatrix = nullptr;
};
