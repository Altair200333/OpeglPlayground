#pragma once
#include "GLWindow.h"
#include "RenderLayer.h"
#include "Scene.h"

class GUIRenderer final: public RenderLayer
{
protected:
	void onRender() override
	{
		for(auto& sprite: scene->sprites)
		{
			sprite->viewportW = viewport->width();
			sprite->viewportH = viewport->height();
			sprite->render();
		}
	}
public:
	std::shared_ptr<Scene> scene;
	std::shared_ptr<GLWindow> viewport;
};
