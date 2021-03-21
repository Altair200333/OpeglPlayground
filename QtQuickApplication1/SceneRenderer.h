#pragma once
#include "Scene.h"

class SceneRenderer final
{
public:
	int drawMode = 0;

	void nextDrawMode()
	{
		drawMode = drawMode == 2 ? 0 : drawMode + 1;
	}
	void renderLights(std::shared_ptr<Scene>& scene)
	{
		for (auto& light : scene->lights)
		{
			auto transform = ComponentManager::getComponent<Transform>(scene->lightSourceBlock);
			transform->translate(-transform->position);
			transform->translate(light->position);
			
			ComponentManager::getComponent<Material>(scene->lightSourceBlock)->diffuse = light->color;
			ComponentManager::getComponent<Material>(scene->lightSourceBlock)->isLightSource = true;
			ComponentManager::getComponent<MeshRenderer>(scene->lightSourceBlock)->render(scene->camera, scene->lights);
		}
	}

	void renderTransparentObjects(std::shared_ptr<Scene>& scene)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render(scene->camera, scene->lights);
		glCullFace(GL_FRONT);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render(scene->camera, scene->lights);
	}

	void render(std::shared_ptr<Scene>& scene)
	{
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		for (size_t i = 0; i < scene->objects.size(); ++i)
		{
			if(drawMode == 0)
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render(scene->camera, scene->lights, scene->backround);
			else if(drawMode == 1)
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render(scene->camera, scene->lights, scene->backround);

				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1, -1);

				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);

				glDisable(GL_POLYGON_OFFSET_LINE);
			}
			else
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);
			}
		}
		
		renderLights(scene);
		scene->backround->render(scene->camera);

		renderTransparentObjects(scene);
	}
};